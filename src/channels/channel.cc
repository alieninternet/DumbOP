
#include "macros.h"
#include "utils.h"
#include "channel.h"
#include "flags.h"
#include "games.h"
#include "gamequiz.h"

Channel::Channel(ServerConnection *c,
                 String name, String wanted = "")
  : channelName(name.toLower()), channelTopic(""),
    lockedTopic(false), channelMode(0), channelLimit(0),
    channelKey(""), keepModes(DEFAULT_KEEPMODES), wantedModes(wanted), 
    count(0), countOp(0), countVoice(0), count_peak(0), countOp_peak(0),
    countVoice_peak(0), joined(false), doMode(true), gotWho(false), cnx(c)
{
   channelMemory.clear();
   channelBanlist.clear();
   
   if (c->bot->wantedChannels[channelName]) {
      if (c->bot->wantedChannels[channelName]->keep != "")
	keepModes = c->bot->wantedChannels[channelName]->keep;
      
      if (c->bot->wantedChannels[channelName]->mode != "")
	wantedModes = c->bot->wantedChannels[channelName]->mode;
   }
   
   /* If this is a quiz channel, we will want to add it to the quiz channel
    * descriptor list
    */
   if ((c->bot->wantedChannels[channelName]->flags & CHANFLAG_ALLOW_GAMES) &&
       (c->bot->wantedChannels[channelName]->gameflags & GAMEFLAG_QUIZ) &&
       !(c->bot->games->quiz->channels[channelName])) {
      c->bot->games->quiz->channels[channelName] = 
	new gameQuizChannel(this, c->bot->games->quiz);
   }
}

Channel::~Channel()
{
  User *u;
  map<String, User *, less<String> >::iterator it;

  while (channelMemory.size() != 0) {
    it = channelMemory.begin();
    u = (*it).second;
    if (u && u->userListItem && u->userListItem->identified)
      u->userListItem->identified = false;
    channelMemory.erase(it);
    delete u;
 }

  BanEntry *b;
  vector<BanEntry *>::iterator it2;

  while (channelBanlist.size() != 0) {
    it2 = channelBanlist.begin();
    b = *it2;
    channelBanlist.erase(it2);
    delete b;
  }

   /* If this was a quiz channel, we need to kill off the quiz details,
    * unfortunately :( */
//   if ((cnx->bot->wantedChannels[channelName]->flags & CHANFLAG_ALLOW_GAMES) &&
//       (cnx->bot->wantedChannels[channelName]->gameflags & GAMEFLAG_QUIZ)) {
//      gameQuizChannel *gqc = cnx->bot->games->quiz->channels[channelName];
//      cnx->bot->games->quiz->channels.erase(channelName);
//      delete gqc;
//   }
}

void Channel::addNick(String n, String uh, int mode, UserList *ul,
		      bool names)
{
   if (User * u = getUser(n.toLower())) {
      count--;
      if (u->mode & User::OP_MODE)
	countOp--;
      if (u->mode & User::VOICE_MODE)
	countVoice--;
      channelMemory.erase(n.toLower());
      delete u;
   }
   
   User *u;
   
   if (names)
     u = new User(n, mode);
   else
     u = new User(n, uh, channelName, mode, ul);
   
   channelMemory[n.toLower()] = u;
   
   if (u->userListItem) {
      if (u->userListItem->identified)
	u->userListItem->identified = true;
      else {
	 if (u->userListItem->passwd == "")
	   u->userListItem->identified = true;
      }
      u->userListItem->lastseen = 0;
   }
   
   count++;
   if (count > count_peak)
     count_peak = count;
   if (u->mode & User::OP_MODE) {
      countOp++;
      if (countOp > countOp_peak)
	countOp_peak = countOp;
   }
   if (u->mode & User::VOICE_MODE) {
      countVoice++;
      if (countVoice > countVoice_peak)
	countVoice_peak = countVoice;
   }
}

void Channel::delNick(String n)
{
   n = n.toLower();
   User *u = getUser(n);
   
   if (!u)
     return;
   
   if (channelMemory[n]!=0) {
      channelMemory.erase(n);
   }
   
   count--;
   if (u->mode & User::OP_MODE)
     countOp--;
   if (u->mode & User::VOICE_MODE)
     countVoice--;
   if (u->userListItem) {
      if (u->userListItem->identified)
	u->userListItem->identified = false;
      u->userListItem->lastseen = cnx->bot->currentTime.time;
   }
   delete u;  
}

void Channel::changeNick(String on, String nn)
{
   on = on.toLower();
   User *u = getUser(on);
   channelMemory.erase(on);
   channelMemory[nn.toLower()] = u;
}

bool Channel::hasNick(String n)
{
   return getUser(n.toLower()) != 0;
}

User * Channel::getUser(String n)
{
   User *u;
   
   n = n.toLower();
   
   if ((u = channelMemory[n]) == 0) {
      channelMemory.erase(n);
      return 0;
   }
   
   return u;
}

void Channel::addBan(String mask, time_t expiration)
{
   for (vector<BanEntry *>::iterator it = channelBanlist.begin();
	it != channelBanlist.end(); ++it)
     if ((*it)->getMask() == mask) {
	if (expiration > (*it)->getExpirationDate())
	  (*it)->setExpirationDate(expiration);
	return;
     }
   channelBanlist.push_back(new BanEntry(mask, expiration));
}

void Channel::delBan(String mask)
{
   for (vector<BanEntry *>::iterator it = channelBanlist.begin();
	it != channelBanlist.end(); ++it)
     if (mask == (*it)->getMask()) {
	BanEntry *b = *it;
	channelBanlist.erase(it);
	delete b;
	break;
     }
}

void Channel::resynchModes()
{
   cnx->queue->sendChannelMode(String("MODE ") + channelName +
			       " +" + cnx->bot->wantedChannels[channelName]->mode);
}

void Channel::parseMode(Person *from, String mode)
{
   char sign = '-';
   StringTokens st(mode);
   String m = st.nextToken(), n;
   User *u = 0;

   if (from)
     u = getUser(from->getNick());
   
   // u == 0 if its a server mode
   bool doNotObey = ((!u) || 
		     (u->getProt() <= User::NO_PROT));

   if (!gotWho)
     doNotObey = false;
   
   for (int i = 0; i < m.length(); i++)
     switch(m[i]) {
      case '+':
      case '-':
	sign = m[i];
	break;
      case 'p': sign =='+' ? channelMode |= PRIVATE :
	channelMode &= ~PRIVATE;
/*	if (keepModes.find('p') != -1 && doNotObey) {
 *	   if (sign == '-' && wantedModes.find('p') != -1)
 *	     cnx->queue->sendChannelMode(channelName, "+p", "");
 *	   else
 *	     cnx->queue->sendChannelMode(channelName, "-p", "");
 *	} */
	break;
      case 's': sign =='+' ? channelMode |= SECRET :
	channelMode &= ~SECRET;
/*	if (keepModes.find('s') != -1 && doNotObey) {
 *	   if (sign == '-' && wantedModes.find('s') != -1)
 *	     cnx->queue->sendChannelMode(channelName, "+s", "");
 *	   else
 *	     cnx->queue->sendChannelMode(channelName, "-s", "");
 *	} */
	break;
      case 'i': sign =='+' ? channelMode |= INVITE_ONLY :
	channelMode &= ~INVITE_ONLY;
/*	if (keepModes.find('i') != -1 && doNotObey) {
 *	   if (sign == '-' && wantedModes.find('i') != -1)
 *	     cnx->queue->sendChannelMode(channelName, "+i", "");
 *	   else
 *	     cnx->queue->sendChannelMode(channelName, "-i", "");
 *	} */
	break;
      case 't': sign =='+' ? channelMode |= TOPIC_RESTRICTED :
	channelMode &= ~TOPIC_RESTRICTED;
/*	if (keepModes.find('t') != -1 && doNotObey) {
 *	   if (sign == '-' && wantedModes.find('t') != -1)
 *	     cnx->queue->sendChannelMode(channelName, "+t", "");
 *	   else
 *	     cnx->queue->sendChannelMode(channelName, "-t", "");
 *	} */
	break;
      case 'n': sign =='+' ? channelMode |= EXTMSG_RESTRICTED :
	channelMode &= ~EXTMSG_RESTRICTED;
/*	if (keepModes.find('n') != -1 && doNotObey) {
 *	   if (sign == '-' && wantedModes.find('n') != -1)
 *	     cnx->queue->sendChannelMode(channelName, "+n", "");
 *	   else
 *	     cnx->queue->sendChannelMode(channelName, "-n", "");
 *	} */
	break;
      case 'm': sign =='+' ? channelMode |= MODERATED :
	channelMode &= ~MODERATED;
/*	if (keepModes.find('m') != -1 && doNotObey) {
 *	   if (sign == '-' && wantedModes.find('m') != -1)
 *	     cnx->queue->sendChannelMode(channelName, "+m", "");
 *	   else
 *	     cnx->queue->sendChannelMode(channelName, "-m", "");
 *	} */
	break;
      case 'l':
/*	if (keepModes.find('l') != -1 && doNotObey) {
 *	   if (sign == '-' && wantedModes.find('l') != -1)
 *	     cnx->queue->sendChannelMode(channelName, "+l",
 *					 String((long)channelLimit));
 *	   else
 *	     cnx->queue->sendChannelMode(channelName, "-l", "");
 *	} */
	channelLimit = (sign == '+' ? atoi(st.nextToken()) :
			channelLimit = 0);
	break;
      case 'k':
	channelKey = st.nextToken();
/*	if (keepModes.find('k') != -1 && doNotObey) {
 *	   if (sign == '-' && wantedModes.find('k') != -1)
 *	     cnx->queue->sendChannelMode(channelName, "+k", channelKey);
 *	   else
 *	     cnx->queue->sendChannelMode(channelName, "-k", channelKey);
 *	} */
	cnx->bot->wantedChannels[channelName]->key = (sign == '+' ? channelKey :
						      String(""));
	break;
      case 'o':
	n = st.nextToken();
	u = getUser(n);
	if (joined) {
	   if (sign == '+') {
	      countOp++;
	      if (countOp > countOp_peak)
		countOp_peak = countOp;
	   } else
	     countOp--;
	}
	if (!u)
	  break;
	sign == '+' ? (u->mode |= User::OP_MODE) :
        (u->mode &= ~User::OP_MODE);
/*	if (sign == '-' && u->getProt() >= User::NO_DEOP) {
 *	   String fromNick = from->getNick();
 *	   User * v = getUser(fromNick);
 *	   if (u == v)
 *	     return;
 *	   if (v->getProt() < User::NO_DEOP)
 *	     cnx->queue->sendChannelMode(channelName, "-o", fromNick);
 *	   cnx->queue->sendChannelMode(channelName, "+o", n);
 *	}
 *	if (sign == '+') {
 *	   if (doNotObey && !from &&
 *	       u->getProt() < User::NO_DEOP && !u->getAop())
 *	     cnx->queue->sendChannelMode(channelName, "-o", n);
 *	   
 *	   if (n.toLower() == cnx->bot->nickName.toLower() && doMode) {
 *	      doMode = false;
 *	      resynchModes();
 *	   }
 *	} */
	break;
      case 'v':
	u = getUser(st.nextToken());
	u->mode = (sign == '+' ? u->mode |= User::VOICE_MODE :
		   u->mode &= ~User::VOICE_MODE);
	if (joined) {
	   if (sign == '+') {
	      countVoice++;
	      if (countVoice > countVoice_peak)
		countVoice_peak = countVoice;
	   } else
	     countVoice--;
	}
	break;
      case 'b':
	String m = st.nextToken();
	sign == '+' ? addBan(m) : delBan(m);
	break;
     }
}

/* sendNotice - Quick way of sending notices to a channel
 * Original 13/07/00, Pickle <pickle@alien.net.au>
 */
void Channel::sendNotice(String message)
{
   cnx->queue->sendNotice(channelName, message);
}

