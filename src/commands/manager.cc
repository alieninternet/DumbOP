
#include "commands.h"
#include "usercommands.h"
#include "flags.h"

/* AddUser
 * Original 18/12/00, Pickle <pickle@alien.net.au>
 */
void
UserCommands::AddUser(ServerConnection *cnx, Person *from,
                      String channel, String rest)
{
  StringTokens st(rest);
  String mask, who, maskChannel, level, prot,
    nick, flags, passwd;

  mask = who = st.nextToken();
  maskChannel = st.nextToken();
  level = st.nextToken();
  prot = st.nextToken();
  nick = st.nextToken();
  flags = st.nextToken();
  passwd = st.nextToken();

  if (mask == "" || maskChannel == "" || level == "" ||
      prot == "" || nick == "") {
    from->sendNotice("\002Invalid syntax for this command.\002");
    return;
  }

  if (!Utils::isWildcard(mask)) {
    mask = cnx->bot->getUserhost(channel, who);
    if (mask == "") {
      from->sendNotice(String("\002I can not find\002 ") + who);
      return;
    }
    mask = Utils::makeWildcard(mask);
  }

  if (cnx->bot->userList->isInUserList(mask, maskChannel)) {
    from->sendNotice(who + " \002is already in userlist on channel(s)\002 " +
                     maskChannel);
    return;
  }

  int l, p;
  long f;

  l = atoi((const char *)level);
  if (l < 0 || l > User::MANAGER)
    return;
  if (l > Utils::getLevel(cnx->bot, from->getAddress())) {
    from->sendNotice("\002You can not give a level greater than yours.\002");
    return;
  }
  p = atoi((const char *)prot);
  if (p < 0 || p > User::NO_DEOP)
    return;

//  f = Utils::strToFlags(flags);
//
//  if (!f)
    f = 0;

  cnx->bot->userList->addUser(mask, maskChannel, l, p, nick, f, -1, passwd);

  from->sendNotice(String("\002Added\002 ") + mask +
                   " \002on channels\002 " + maskChannel);
  from->sendNotice(String("\002Level:\002 ") +
                   Utils::levelToStr(l) +
                   "  \002Protection:\002 " +
                   Utils::protToStr(p));

  cnx->bot->rehash();
}

/* Alias
 * Original 18/12/00, Pickle <pickle@alien.net.au>
 */
void
UserCommands::Alias(ServerConnection *cnx, Person *from,
                    String channel, String rest)
{
  StringTokens st(rest);
  String newF = st.nextToken().toUpper();
  String oldF = st.nextToken().toUpper();
  list<userFunction *>::iterator it;

  if (newF == "" || oldF == "") {
    from->sendNotice("\002Invalid syntax for this command.\002");
    return;
  }

  // First, we check that the "new" function does not exist
  for (it = cnx->bot->userFunctions.begin(); it !=
         cnx->bot->userFunctions.end(); ++it)
    if (newF == (*it)->name) {
      from->sendNotice(newF + " \002is already an alias.\002");
      return;
    }

  // Next, we check that the "old" function exist
  bool found = false;
  userFunction *u;
  for (it = cnx->bot->userFunctions.begin(); it !=
         cnx->bot->userFunctions.end(); ++it)
    if (oldF == (*it)->name) {
      found = true;
      u = *it;
      break;
    }
  if (!found) {
    from->sendNotice(String("\002I don't know the\002 ") + oldF +
                     " \002command.");
    return;
  }

  // Fine, we do the binding
  cnx->bot->userFunctions.push_back(new
                               userFunction((char *)(const char 
                                                            *)newF,
                                                   u->function,
                                                   u->minLevel,
                                                   u->needsChannelName));

  from->sendNotice("\002Alias added.\002");
}

/* DelUser
 * Original 18/12/00, Pickle <pickle@alien.net.au>
 */
void
UserCommands::DelUser(ServerConnection *cnx, Person *from,
                      String channel, String rest)
{
  StringTokens st(rest);

  String who;
  String mask = who = st.nextToken();
  String maskChannel = st.nextToken();

  if (mask == "" || maskChannel == "") {
    from->sendNotice("\002Invalid syntax for this command.\002");
    return;
  }

  if (!Utils::isWildcard(mask)) {
    mask = cnx->bot->getUserhost(channel, who);
    if (mask == "") {
      from->sendNotice(String("\002I can not find\002 ") + who);
      return;
    }
    mask = Utils::makeWildcard(mask);
  }
  
  if (!cnx->bot->userList->isInUserList(mask, maskChannel)) {
    from->sendNotice(mask + " \002is not in userlist on channel(s)\002 " +
                     maskChannel);
    return;
  }

  cnx->bot->userList->removeUser(mask, maskChannel);
  from->sendNotice(who + " \002has been removed from the userlist.\002");
  cnx->bot->rehash();
}

/* Names - List names on a channel, or channels
 * Original 23/12/00, Pickle <pickle@alien.net.au>
 * Needs: Channel authorisation checking
 */
void UserCommands::Names(ServerConnection *cnx, Person *from,
			 String channel, String rest)
  {
     String nick = from->getNick();
     String result = "";
     int length = 0;
     
     if (rest == "-")
       for (map<String, Channel *, less<String> >::iterator it = cnx->bot->channelList->begin(); 
	    it != cnx->bot->channelList->end(); ++it) {
	  Channel *c = cnx->bot->channelList->getChannel((*it).first);
	  
	  if (true) {
	     result = "";
	     length = 0;
	     
	     from->sendNotice(String("Names on \002") + c->channelName + 
			      "\002:");
	     
	     for (map<String, User *, less<String> >::iterator itB = 
		  c->channelMemory.begin();
		  itB != c->channelMemory.end(); ++itB) {
		result = result +
		  ((((*itB).second->mode & User::OP_MODE) ?
		    "@" :
		    (((*itB).second->mode & User::VOICE_MODE) ? 
		     "+" : ""))) +
		  (((*itB).second->userListItem) ?
		   "\037" : "" ) +
		  (*itB).second->nick +
		  (((*itB).second->userListItem) ?
		   "\037" : "") + " ";
		length += (*it).first.length() + 1;
		if (length >= 350) {
		   from->sendNotice(result);
		   result = ""; length = 0;
		}
	     }
	     
	     if (result != "")
	       from->sendNotice(result);
	  }
       }
     else {
	Channel *c = cnx->bot->channelList->getChannel(channel);

	from->sendNotice(String("Names on \002") + channel + "\002:");
	
	for (map<String, User *, less<String> >::iterator it = 
	     c->channelMemory.begin();
	     it != c->channelMemory.end(); ++it) {
	   result = result +
	     ((((*it).second->mode & User::OP_MODE) ?
	       "@" :
	       (((*it).second->mode & User::VOICE_MODE) ? 
		"+" : ""))) +
	     (((*it).second->userListItem) ?
	      "\037" : "" ) +
	     (*it).second->nick +
	     (((*it).second->userListItem) ?
	      "\037" : "") + " ";
	   length += (*it).first.length() + 1;
	   if (length >= 350) {
	      from->sendNotice(result);
	      result = ""; length = 0;
	   }
	}
	
	if (result != "")
	  from->sendNotice(result);
     }
     
     from->sendNotice("\002End of names.\002");
  }

/* Save - Force a database save
 * Original 15/12/00, Pickle <pickle@alien.net.au>
 */
void UserCommands::Save(ServerConnection *cnx, Person *from,
			String channel, String rest)
{
   cnx->bot->userList->save();
   cnx->bot->noteList->save();
   from->sendNotice("Databases saved.");
}

/* UserList - Display a list of users
 * Original 14/12/00, Pickle <pickle@alien.net.au>
 * 22/12/00 Pickle - Reformatted
 */
void UserCommands::UserList(ServerConnection *cnx, Person *from,
			    String channel, String rest)
  {
     int num = 0;
     
     from->sendNotice("\002Userlist:\002");
     from->sendNotice("\026 \037Nickname       \037 \037Mask            \037 \037Channel   \037 \037Level       \037 \037Flags     \037 \026");
     
     for (list<UserListItem *>::iterator it = cnx->bot->userList->l.begin();
	  it != cnx->bot->userList->l.end();
	  it++) {
	from->sendNotice((((*it)->lastseen == 0) ?
			  (((*it)->flags & PERSONFLAG_IDENTIFIED) ?
			   String("\002*\002") :
			   String("\002+\002")) : 
			  String(" ")) +
			 Utils::getFirstNick((*it)->nick).prepad(15) + " " +
			 String("\002[\002") +
			 (*it)->mask.getMask().pad(14) + String("\002]\002 ") +
			 (*it)->channelMask.getMask().pad(10) + " " +
			 Utils::levelToStr((*it)->level).pad(12) + " " +
			 Utils::flagsToStr((*it)->flags).pad(10) + " " +
			 String((*it)->flags));
	num++;
     }
     
     if (num == 0) 
       from ->sendNotice("\002End of userlist\002 (No matches)");
     else 
       from->sendNotice(String("\002End of userlist\002 (") +
			String((int)num) + " entries)");
  }


/* MOVE COMMENTED CODE TO SETUSER */
// void
// UserCommands::ChangeLevel(ServerConnection *cnx, Person *from,
//                           String channel, String rest)
// {
//   StringTokens st(rest);
  
//   String who;
//   String mask = who = st.nextToken();
//   String maskChannel = st.nextToken();
//   String level = st.nextToken();
  
//   if (mask == "" || maskChannel == "" || level == "") {
//     from->sendNotice("\002Invalid syntax for this command.\002");
//     return;
//   }
  
//   if (!Utils::isWildcard(mask)) {
//     mask = cnx->bot->getUserhost(channel, who);
//     if (mask == "") {
//       from->sendNotice(String("\002I can not find\002 ") + who);
//       return;
//     }
//     mask = from->getNick() + "!" + mask;
//   }
  
//   UserListItem *uli = cnx->bot->userList->getUserListItem(mask, maskChannel);
  
//   if (!uli) {
//     from->sendNotice(String("\002I can not find\002 ") + who +
//                      " \002on channel(s)\002 " + maskChannel +
//                      " \002in my userlist.\002");
//     return;
//   }
  
//   int l = atoi((const char *)level);
  
//   if (l < User::NONE || l > User::MASTER) {
//     from->sendNotice("\002This is not a valid level.\002");
//     return;
//   }
  
//   if (l > Utils::getLevel(cnx->bot, from->getAddress())) {
//     from->sendNotice("\002You can not give a level greater than yours.\002");
//     return;
//   }
  
//   if (Utils::getLevel(cnx->bot, from->getAddress()) < uli->level) {
//     from->sendNotice("\002You can not change the level for a person "
//                      "whose level is greater than yours.\002");
//     return;
//   }
  
//   uli->level = l;
//   from->sendNotice("\002Level changed.\002");
//   cnx->bot->rehash();
// }


