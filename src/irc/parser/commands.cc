
#include "config.h"
#include "parser.h"
#include "bot.h"
#include "serverqueue.h"
#include "flags.h"
#include "user.h"
#include "utils.h"

void Parser::parseError(Person *from, String rest)
{
   from->cnx->bot->logLine(String("Error from server ") +
			   from->cnx->bot->serverList->currentServer()->getHostName() +
			   " (" + 
			   String((long)from->cnx->bot->serverList->currentServer()->getPort()) +
			   ").");
   from->cnx->bot->nextServer();
}


void Parser::parseInvite(Person *from, String rest)
{
   String nick = from->getNick();
   StringTokens st(rest);
   st.nextToken(':');
   String channel = st.rest();
   
   if (from->cnx->bot->wantedChannels.find(channel) !=
       from->cnx->bot->wantedChannels.end()) {
      from->cnx->queue->sendJoin(channel, from->cnx->bot->wantedChannels[channel]->key);
   }
}


void Parser::parseJoin(Person *from, String rest)
{
   StringTokens st(from->getAddress());
   String n = st.nextToken('!');
   String uh = st.nextToken();
   StringTokens st2(rest);
   String c = st2.nextToken(':');
   String mode;
   bool joinAndMode = false;
   
   // This part of code is for the combined JOIN & MODE of ircd 2.9
   if (c.find('\007') >= 0) {
      joinAndMode = true;
      StringTokens st3(c);
      c = st3.nextToken('\007');
      String m = st3.rest();
      mode = c + " +" + m;
      
      for (int i = 0; i < m.length(); i++) {
	mode = mode + " " + n;    
      }
   }
   
   if (n == from->cnx->bot->nickName) {
      from->cnx->bot->logLine(String("Joined channel ") + c + ".");
      
      if (from->cnx->bot->wantedChannels.find(c) != from->cnx->bot->wantedChannels.end()) {
	from->cnx->bot->channelList->addChannel(from->cnx, c, from->cnx->bot->wantedChannels[c]->keep);
      } else {
	 from->cnx->bot->channelList->addChannel(from->cnx, c);
      }
      
      from->cnx->queue->sendWho(c);
      from->cnx->queue->sendChannelMode(String("MODE ") + c + " b");
      from->cnx->queue->sendChannelMode(String("MODE ") + c);
   } else {
      Channel * ch = from->cnx->bot->channelList->getChannel(c);
      
      // Make sure we got the channel, just in case
      if (!ch) {
	 return;
      }

      // Add this nickname to the channel
      ch->addNick(n, uh, 0, from->cnx->bot->userList);
      
      // Check if we have a greeting message we should send this user
      if (from->cnx->bot->wantedChannels[ch->channelName]->greeting.length()) {
	 from->sendNotice(from->cnx->bot->wantedChannels[ch->channelName]->greeting);
      }
      
      // If it is a game channel, the game engine may need this data too.
      if (from->cnx->bot->wantedChannels[ch->channelName]->flags & 
	  CHANFLAG_ALLOW_GAMES) {
	 from->cnx->bot->games->parseJoin(ch, from);
	 
	 // Check if this channel is also allowed to do 'credit finding'
	 if (from->cnx->bot->wantedChannels[ch->channelName]->flags &
	     CHANFLAG_ALLOW_FIND_CREDITS) {
	    /* Check if we got the user, if they are identified, and if the
	     * user has 'won' some credits
	     */
	    if ((from->uli) && (from->uli->identified) &&
		!((from->uli->flags & USERFLAG_HAS_MONEY_TREE) ||
		  (from->uli->level >= User::MANAGER)) &&
		(Utils::random(100) <= CHANNEL_JOIN_FIND_PERCENTAGE)) {
	       // Get the amount they found/won
	       short amount = (Utils::random(CHANNEL_JOIN_MAX_CREDIT_FIND) + 
			       1);
	       
	       // Tell the user what they found
	       from->sendLine(String("I felt nice, and have given you \002") +
			      String(amount) + String("\002 credits!"));
	       
	       // Give the credits to the user
	       from->uli->pay(amount);
	    }
	 }
      }
   }
   
   if (joinAndMode) {
      parseMode(from, mode);
   }
}


void Parser::parseKick(Person *from, String rest)
{
   StringTokens st(rest);
   String channel = st.nextToken();
   String target = st.nextToken();
   String reason = st.rest().subString(1);
   
   if (target == from->cnx->bot->nickName) {
      from->cnx->bot->logLine(from->getAddress() + " kicked me out of channel " +
			channel + " (" + reason + ").");
      from->cnx->queue->sendJoin(channel, from->cnx->bot->channelList->getChannel(channel)->channelKey);
      from->cnx->bot->channelList->delChannel(channel);
   } else {
      if (!from->cnx->bot->channelList->getChannel(channel)) {
	 return;
      }
      
      User *u = from->cnx->bot->channelList->getChannel(channel)->getUser(target);
    
      if (u && u->getProt() >= User::NO_KICK) {
	 String fromNick = from->getNick();
	 User *v = from->cnx->bot->channelList->getChannel(channel)->getUser(fromNick);
	 if (v->getProt() < User::NO_KICK) {
	    from->cnx->bot->logLine(from->getAddress() + " kicked " + target +
			      " (protected) out of channel " + channel +
			      " (" + reason + ").");
	    from->cnx->queue->sendKick(channel, fromNick,
				 target + " \002is protected !\002");
	 }
      }

      // Delete the nickname from that channel
      from->cnx->bot->channelList->getChannel(channel)->delNick(target);
   }
}


void Parser::parseMode(Person *from, String rest)
{
   StringTokens st(rest);
   String ch = st.nextToken();
   String modes = st.rest();
   
   if (Utils::isChannel(ch)) {
      Channel *c = from->cnx->bot->channelList->getChannel(ch);
      
      if (!c) {
	 return;
      }
      
      if (from) {
	 c->parseMode(from, modes);
      } else {
	 c->parseMode(0, modes);
      }
   }
}


void Parser::parseNick(Person *from, String rest)
{
   String oldNick = from->getNick();
   String newNick = rest.subString(1);
   
   for (map<String, Channel *, less<String> >::iterator it = 
	from->cnx->bot->channelList->begin(); 
	it != from->cnx->bot->channelList->end(); ++it) {
      if ((*it).second->hasNick(oldNick)) {
	 (*it).second->changeNick(oldNick, newNick);
      }
   }
}


void Parser::parseNotice(Person *from, String rest)
{
   String nick = "";
   
  if (from) {
     nick = from->getNick();
  }
   
   StringTokens st(rest);
   String to = st.nextToken();
   
   rest = st.rest().subString(1);
   
   if (rest[0] != '\001') {
    return;
   }
   
   rest = rest.subString(1, rest.length() - 2);
   StringTokens st2(rest);
   String command = st2.nextToken();
   rest = st2.rest();

   if (command == "PING") {
      StringTokens st3(rest);
      time_t time_secs = atol(st3.nextToken(':'));
      unsigned short time_usecs = atoi(st3.rest());
      
      /* The internals of this process can be big -- eg. long long */
      long long pingpong = (((from->cnx->bot->currentTime.time * 1000) + 
			     from->cnx->bot->currentTime.millitm) -
			    ((time_secs * 1000) + time_usecs));
      
      from->sendLine(String("Ping shows a lag between us of \002") +
		     Utils::timeBigToStr((long)pingpong) +
		     String("\002") +
		     ((from->cnx->lag > 0) ? 
		      (String(" (My connection is lagged by \002") +
		       Utils::timeBigToStr(from->cnx->lag) + String("\002)")) :
		      (String(""))));
   }
}


void Parser::parsePrivmsg(Person *from, String rest)
{
   String nick = from->getNick();
   
   StringTokens st(rest);
   String to = st.nextToken();
   String fromUserhost = Utils::getUserhost(from->getAddress());
   
   rest = st.rest().subString(1);
   
   if (++(from->cnx->bot->ignoredUserhosts[fromUserhost]) > Bot::MAX_MESSAGES) {
      if (from->cnx->bot->ignoredUserhosts[fromUserhost] == Bot::MAX_MESSAGES+1) {
	 from->cnx->bot->ignoredUserhosts[fromUserhost] += Bot::IGNORE_DELAY;
	 from->cnx->bot->logLine(from->getAddress() +
			   " is flooding me. We will ignore him/her/it.");
	 if (!Utils::isChannel(to)) {
	    from->sendLine(String("Stop flooding me -- You are now being ignored for ") +
			   String((long)Bot::IGNORE_DELAY) + " seconds.\002");
	 }
      }
      // The following lines reset the counter if you use the
      // command "!sorry" (if '!' is your command char).
      if (rest.toUpper() == String(from->cnx->bot->commandChar) + "SORRY") {
	 from->cnx->bot->ignoredUserhosts[fromUserhost] = 0;
	 from->sendLine("\002Don't do it again!\002");
      }
      return;
   }
   
   if (rest[0] == '\001') {
      rest = rest.subString(1, rest.length() - 2);
#ifdef NOT_VERY_HAPPY_ABOUT_CHANNEL_CTCP_BROADCASTS_WORKING_NO_SIREE_BOB
      if (!Utils::isChannel(to)) 
#endif
	Parser::parseCTCP(from, to, rest);
   } else {
      Parser::parseMessage(from, to, rest);
   }
}


void Parser::parsePart(Person *from, String rest)
{
   String n = from->getNick();
   StringTokens st(rest);
   String channel = st.nextToken();
   
   if (n.toLower() == from->cnx->bot->nickName.toLower()) {
      from->cnx->bot->logLine(String("Left channel ") + channel + ".");
      from->cnx->bot->channelList->delChannel(channel);
   } else {
      Channel * c = from->cnx->bot->channelList->getChannel(channel);
      
      if (!c) {
	 return;
      }
      
      c->delNick(n);
      
      if (c->countOp == 0 && c->count == 1) {
	 from->cnx->queue->sendPart(channel, "");
	 from->cnx->queue->sendJoin(channel, from->cnx->bot->wantedChannels[channel]->key);
      }

      // If it is a game channel, the game engine may need this data too.
      if (from->cnx->bot->wantedChannels[c->channelName]->flags & 
	  CHANFLAG_ALLOW_GAMES) {
	 from->cnx->bot->games->parseLeave(c, from);
      }
   }
}


void Parser::parsePing(Person *from, String rest)
{
   from->cnx->queue->sendPong(rest);
}


void Parser::parsePong(Person *from, String rest)
{
   from->cnx->lag = (((from->cnx->bot->currentTime.time * 1000) + 
		from->cnx->bot->currentTime.millitm) -
	       ((from->cnx->pingTime.time * 1000) +
		from->cnx->pingTime.millitm));
   from->cnx->bot->sentPing = false;
}


void Parser::parseQuit(Person *from, String rest)
{
   String n = from->getNick();
   
   if (n == from->cnx->bot->nickName) {
      from->cnx->bot->stop = true;
   }
   
   for (map<String, Channel *, less<String> >::iterator it =
	from->cnx->bot->channelList->begin();
	it != from->cnx->bot->channelList->end();
	++it) {
      (*it).second->delNick(n);
   }
}


void Parser::parseTopic(Person *from, String rest)
{
   StringTokens st(rest);
   String channel = st.nextToken();
   String newTopic = st.rest().subString(1);
   Channel *c = from->cnx->bot->channelList->getChannel(channel);
   
  if (!c) {
     return;
  }

   if (c->lockedTopic && from->getNick() != from->cnx->bot->nickName) {
      from->cnx->queue->sendTopic(channel, c->channelTopic);
   }
   
   c->channelTopic = newTopic;
}


void Parser::parseWallops(Person *from, String rest)
{
   from->cnx->bot->logLine(String("WALLOPS from ") + from->getAddress() +
			   String(" (") + rest + String(")"));
}
