
#include <fstream.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ctype.h>
#ifndef _X_OPEN_SOURCE
#define _X_OPEN_SOURCE
#endif
#ifndef _X_OPEN_SOURCE_EXTENDED
#define _X_OPEN_SOURCE_EXTENDED 1
#endif
#ifndef __USE_XOPEN
#define __USE_XOPEN
#endif
#include <unistd.h>
#include <time.h>

#include "commands.h"
#include "usercommands.h"
#include "dccconnection.h"
#include "parser.h"
#include "macros.h"
#include "stringtokenizer.h"
#include "string.h"
#include "utils.h"
#include "channellist.h"
#include "serverlist.h"
#include "server.h"
#include "bot.h"
#include "flags.h"
#include "version.h"
#include "games.h"
#include "gamequiz.h"

#ifdef NOCRYPT
char * crypt(const char *p, const char *s) { return p; }
#endif

/* Access
 * Original 20/12/00, Pickle <pickle@alien.net.au>
 */
void UserCommands::Access(ServerConnection *cnx, Person *from,
			  String channel, String rest)
{
   String nick = from->getNick();
   
   if (!rest.length()) {
      User * u = cnx->bot->channelList->getChannel(channel)->getUser(nick);
      UserListItem * uli;
      
      if (u)
	uli = u->userListItem;
      else
	uli = cnx->bot->userList->getUserListItem(from->getAddress(),
						  channel);
      
      if (uli) {
	 from->sendNotice(String("You are \002") +
			  uli->mask.getMask() + "\002 on \002" +
			  uli->channelMask.getMask() + "\002");
	 from->sendNotice(String("Lvl: ") +
			  Utils::levelToStr(uli->level) +
			  " Prot: " +
			  Utils::protToStr(uli->prot) +
			  " Ident: " +
			  (uli && !uli->identified ? "\026" : "") +
			  Utils::boolToStr(uli && uli->identified) +
			  (uli && !uli->identified ? "\026" : ""));
      } else
	from->sendNotice(String("You are not in the userlist for \002") +
			 channel + "\002");
   } else {
      StringTokenizer st(rest);
      String otherNick = st.nextToken();
      
      if (otherNick == "-") {
	 int num = 0, num_on = 0;
	 
	 from->sendNotice(String("Access list search for mask \002") +
			  otherNick + String("\002 on \002") +
			  channel + String("\002:"));
	 from->sendNotice("\026 \037Nickname       \037 \037Level       \037 \037A+o\037 \037A+v\037 \037Right\037 \037Ident\037 \037Flags     \037 \026");
	 
	 for (list<UserListItem *>::iterator it = cnx->bot->userList->l.begin();
	      it != cnx->bot->userList->l.end();
	      it++) {
	    if (((*it)->channelMask.getMask().toLower() == channel.toLower()) &&
		!((*it)->flags & USERFLAG_SUSPENDED)) {
	       from->sendNotice((((*it)->lastseen == 0) ?
				 (((*it)->flags & PERSONFLAG_IDENTIFIED) ?
				  String("\002*\002") :
				  String("\002+\002")) : 
				 String(" ")) +
				Utils::getFirstNick((*it)->nicks).prepad(15) + " " +
				Utils::levelToStr((*it)->level).pad(12) + " " +
				Utils::boolToStr((*it)->flags & USERFLAG_JOIN_AOP).pad(3) + " " +
				Utils::boolToStr((*it)->flags & USERFLAG_JOIN_AOV).pad(3) + " " +
				(((*it)->flags & PERSONFLAG_IS_BOT) ?
				 String("Bot") :
				 (((*it)->flags & USERFLAG_IS_CHAN_OWNER) ?
				  String("Admin") : String("Norm."))).pad(5) + " " +
				Utils::boolToStr((*it)->flags & PERSONFLAG_IDENTIFIED).pad(5) + " " +
				Utils::flagsToStr((*it)->flags).pad(10));
	       num++;
	       if ((*it)->lastseen == 0)
		 num_on++;
	    }
	 }
	 
	 if (num == 0) 
	   from ->sendNotice("\002End of access list\002 (No matches)");
	 else 
	   from->sendNotice(String("\002End of access list\002 (") +
			    String((int)num) + " entries, " + 
			    String((int)num_on) + " online)");
      } else {
	 User * u = cnx->bot->channelList->getChannel(channel)->getUser(otherNick);
	 UserListItem * uli;
	 
	 if (u)
	   uli = u->userListItem;
	 else
	   uli = cnx->bot->userList->getUserListItem(otherNick + "!" +
						     cnx->bot->getUserhost(channel,
									   otherNick),
						     channel);
	 
	 if (uli) {
	    from->sendNotice(String("\002") + otherNick +
			     String("\002 is \002") + uli->mask.getMask() +
			     String("\002 on \002") + uli->channelMask.getMask() + 
			     String("\002"));
	    from->sendNotice(String("Lvl: ") +
			     Utils::levelToStr(uli->level) +
			     " Prot: " +
			     Utils::protToStr(uli->prot) +
			     " Ident: " +
			     Utils::boolToStr(uli && uli->identified));
	 } else
	   from->sendNotice(String("\002") + otherNick +
			    String("\002 is not in the userlist for \002") +
			    channel + String("\002"));
      }
   }
}

/* AddUser
 * Original 18/12/00, Pickle <pickle@alien.net.au>
 */
void
UserCommands::AddUser(ServerConnection *cnx, Person *from,
                      String channel, String rest)
{
  StringTokenizer st(rest);
  String mask, who, maskChannel, level, prot,
    nicks, flags, passwd;

  mask = who = st.nextToken();
  maskChannel = st.nextToken();
  level = st.nextToken();
  prot = st.nextToken();
  nicks = st.nextToken();
  flags = st.nextToken();
  passwd = st.nextToken();

  if (mask == "" || maskChannel == "" || level == "" ||
      prot == "" || nicks == "") {
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

  cnx->bot->userList->addUser(mask, maskChannel, l, p, nicks, f, -1, passwd);

  from->sendNotice(String("\002Added\002 ") + mask +
                   " \002on channels\002 " + maskChannel);
  from->sendNotice(String("\002Level:\002 ") +
                   Utils::levelToStr(l) +
                   "  \002Protection:\002 " +
                   Utils::protToStr(p));

  cnx->bot->rehash();
}

/* AddServer
 * Original 18/12/00, Pickle <pickle@alien.net.au>
 */
void
UserCommands::AddServer(ServerConnection *cnx, Person *from,
                        String channel, String rest)
{
  if (rest.length() == 0) {
    from->sendNotice("\002You must supply a server name.\002");
    return;
  }

  StringTokenizer st(rest);
  String serverName = st.nextToken();
  int port = 6667;

  if (st.hasMoreTokens()) {
    String temp = st.nextToken();
    port = atoi((const char *)temp);
  }

  Message m = Commands::AddServer(cnx->bot, serverName, port);
  if (m.getCode() != 0)
    from->sendNotice(m.getMessage());
  else
    from->sendNotice(String("\002Server\002 ") +
                     serverName + " \002on port\002 " +
                     String((long)port) + " \002has been added "
                     "to the server list.\002");
}

/* Alias
 * Original 18/12/00, Pickle <pickle@alien.net.au>
 */
void
UserCommands::Alias(ServerConnection *cnx, Person *from,
                    String channel, String rest)
{
  StringTokenizer st(rest);
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

/* Ban
 * Original 15/12/00, Pickle <pickle@alien.net.au>
 */
void
UserCommands::Ban(ServerConnection *cnx, Person *from,
                  String channel, String rest)
{
  if (rest.length() == 0) {
    if (from)
      from->sendNotice("\002No nick/mask specified.\002");
    return;
  }

  Message m = Commands::Ban(cnx->bot, channel, rest);
  if (m.getCode() != 0 && from)
    from->sendNotice(m.getMessage());
}

/* BanList
 * Original 15/12/00, Pickle <pickle@alien.net.au>
 */
void
UserCommands::BanList(ServerConnection *cnx, Person *from,
                      String channel, String rest)
{
  Channel *c = cnx->bot->channelList->getChannel(channel);
  from->sendNotice(String("\002Banlist for channel\002 ") +
                   channel + "\002:\002");
  from->sendNotice("\002Mask                           Expires (seconds)\002");
   for (vector<BanEntry *>::iterator it = c->channelBanlist.begin();
	it != c->channelBanlist.end(); ++it) {
      if ((*it)->getExpirationDate() == -1) {
	 from->sendNotice((*it)->getMask().pad(30) + " -1");
      } else {
	 from->sendNotice((*it)->getMask().pad(30) + " " + 
			  String((long)((*it)->getExpirationDate() - 
					cnx->bot->currentTime.time)));
      }
  }
  from->sendNotice("\002End of banlist.\002");
}

/* MOVE COMMENTED CODE TO SETUSER */
// void
// UserCommands::ChangeLevel(ServerConnection *cnx, Person *from,
//                           String channel, String rest)
// {
//   StringTokenizer st(rest);
  
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

/* Cycle
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 */
void UserCommands::Cycle(ServerConnection *cnx, Person *from,
			 String channel, String rest)
{
   Message m = Commands::Cycle(cnx->bot, channel, rest);
   if (m.getCode() != 0)
     from->sendNotice(m.getMessage());
   else
     from->sendNotice(String("Cycled on channel \002") + channel + 
		      String("\002"));
}

/* DCCList
 * Original 19/12/00, Pickle <pickle@alien.net.au>
 */
void
UserCommands::DCCList(ServerConnection *cnx, Person *from,
                      String channel, String rest)
{
  from->sendNotice("\002DCClist:\002");
  from->sendNotice("\002Hostname                         Last used\002");

  for (list<DCCConnection *>::iterator it =
         cnx->bot->dccConnections.begin();
       it != cnx->bot->dccConnections.end();
       ++it) {
    from->sendNotice((*it)->nuh.pad(32) + " " +
                     String((long)(cnx->bot->currentTime.time -
                                   (*it)->lastSpoken)));
  }

  from->sendNotice("\002End of dcclist.\002");
}

/* Deban
 * Original 15/12/00, Pickle <pickle@alien.net.au>
 */
void
UserCommands::Deban(ServerConnection *cnx, Person *from,
                    String channel, String rest)
{
  if (rest.length() == 0) {
    from->sendNotice("\002No nick/mask specified.\002");
    return;
  }

  Message m = Commands::Deban(cnx->bot, channel, rest);
  if (m.getCode() != 0)
    from->sendNotice(m.getMessage());
}

/* DelServer
 * Original 18/12/00, Pickle <pickle@alien.net.au>
 */
void
UserCommands::DelServer(ServerConnection *cnx, Person *from,
                        String channel, String rest)
{
  if (rest.length() == 0) {
    from->sendNotice("\002You need to supply a server number"
                     " for this command.\002");
    return;
  }

  int serverNumber = atoi(rest);

  Message m = Commands::DelServer(cnx->bot, serverNumber);
  if (m.getCode() != 0)
    from->sendNotice(m.getMessage());
  else
    from->sendNotice(String("Deleted server ") +
                     cnx->bot->serverList->get(serverNumber)->getHostName() +
                     " (" + String((long)cnx->bot->serverList->get(serverNumber)->getPort()) +
                     ").");
}

/* DelUser
 * Original 18/12/00, Pickle <pickle@alien.net.au>
 */
void
UserCommands::DelUser(ServerConnection *cnx, Person *from,
                      String channel, String rest)
{
  StringTokenizer st(rest);

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

/* Deop
 * Original 14/12/00, Pickle <pickle@alien.net.au>
 * Needs: Convertion from using commands.c
 */
void UserCommands::Deop(ServerConnection *cnx, Person *from,
			String channel, String rest)
{
   String target = rest;
   
   if (target.length() == 0)
     target = from->getNick();
   
   Message m = Commands::Deop(cnx->bot, channel, target);
   if (m.getCode() != 0)
     from->sendNotice(m.getMessage());
}

/* Die - Make the bot quit
 * Original 13/12/00, Pickle <pickle@alien.net.au>
 * 28/12/00 Pickle - Changed default quit message to VERSION_STRING
 */
void UserCommands::Die(ServerConnection *cnx, Person *from,
		       String channel, String rest)
{
   if (rest.length() == 0)
     Commands::Die(cnx->bot, from->getNick() +
		   String(" requested me to leave [") + 
		   VERSION_STRING + String("]"));
   else
     Commands::Die(cnx->bot, rest);
}

/* Do
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 * Needs: Remove command.c dependancy
 */
void UserCommands::Do(ServerConnection *cnx, Person *from,
		      String channel, String rest)
{
   if (!rest.length())
     from->sendNotice("And do what?");
   else {
      
      Message m = Commands::Action(cnx->bot, channel, rest);
      if (m.getCode() != 0)
	from->sendNotice(m.getMessage());
   }
}

/* Help - Display help
 * Original 15/12/00, Pickle <pickle@alien.net.au>
 * 26/12/00 Pickle - Changed header output depending on command or topic
 */
void UserCommands::Help(ServerConnection *cnx, Person *from,
			String channel, String rest)
{
   if (rest.length() == 0) {
      from->sendNotice("\002Commands available to you are:\002");
      int level = Utils::getLevel(cnx->bot, from->getAddress());
      String result = "";
      int length = 0;
      for (list<userFunction *>::iterator it = cnx->bot->userFunctions.begin(); 
	   it != cnx->bot->userFunctions.end(); ++it)
	if ((*it)->minLevel <= level) {
	   result = result + (*it)->name + " ";
	   length += strlen((*it)->name) + 1;
	   if (length >= 256) {
	      from->sendNotice(result);
	      result = ""; length = 0;
	   }
	}
      if (result != "")
	from->sendNotice(result);
      from->sendNotice(String("Use \002") + String(cnx->bot->commandChar) +
		       "HELP <command>\002 for more information.");
      return;
   }
   
   StringTokenizer st(rest);
   String command = st.nextToken().toUpper();
   ifstream helpFile(cnx->bot->helpFileName);
   
   if (!helpFile) {
      from->sendNotice("I can't find the help file :(");
      return;
   }
   
   String buf;
   while (!helpFile.eof()) {
      helpFile >> buf;
      if (buf.subString(0, command.length()) == String(":") + command) {
	 buf = buf.subString(1);
	 int level = -1;
	 
	 for (list<userFunction *>::iterator it = 
	      cnx->bot->userFunctions.begin(); 
	      it != cnx->bot->userFunctions.end(); ++it)
	   if ((*it)->name == command) {
	      level = (*it)->minLevel;
	      break;
	   }
	 
	 if (level > -1)
	   from->sendNotice(String(String("Help for command \002") +
				   String(cnx->bot->commandChar) +
				   command + String("\002:")).pad(40) + 
			    String("Access Required: \002") +
			    Utils::levelToStr(level) +
			    String("\002"));
	 else
	   from->sendNotice(String(String("Help on topic \002\"") + command +
				   String("\002\":")));
	 
	 while (buf != "") {
	    from->sendNotice(buf);
	    helpFile >> buf;
	 }
	 from->sendNotice("\002End of help.\002");
	 return;
      }
   }

   from->sendNotice(String("Couldn't find \002") +
		    command + "\002 in the helpfile.");
}

/* Ident
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 */
void UserCommands::Ident(ServerConnection *cnx, Person *from,
			 String channel, String rest)
{
   Channel *c = cnx->bot->channelList->getChannel(channel);
   
   if (rest.length() <= 2) {
      from->sendNotice("\002No password specified or password "
		       "too short.\002");
      return;
   }
   
   User * u = c->getUser(from->getNick());
   if (!u) {
      from->sendNotice(String("\002You can identify yourself on"
			      " channel\002 ") + channel +
		       " \002only if you are on the channel.\002");
      return;
   }
   
   if (u->userListItem && u->userListItem->identified) {
      from->sendNotice(String("\002You are already identified on"
			      " channel\002 ") + channel);
      return;
   }
   
   if (!u->userListItem) {
      from->sendNotice("\002You are not in my userlist.\002");
      return;
   }
   
   if (u->userListItem->passwd ==
       crypt((const char *)rest, (const char *)u->userListItem->passwd)) {
      // For each channel, we increment identification counter
      for (map<String, Channel *, less<String> >::iterator it =
           cnx->bot->channelList->begin();
	   it != cnx->bot->channelList->end(); ++it)
	u->userListItem->identified++;
      
      from->sendNotice("\002You are now identified.\002");
   } else
     from->sendNotice("\002This is a wrong password.\002");
}

/* Invite
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 * Needs: Convert from command.c dependancy
 * Needs: Self-invite ability without specifying originating nick
 */
void
UserCommands::Invite(ServerConnection *cnx, Person *from,
                     String channel, String rest)
{
  Message m = Commands::Invite(cnx->bot, channel, rest);
  if (m.getCode() < 0)
    from->sendNotice(m.getMessage());
  from->sendNotice(String("\002Inviting\002 ") + rest + " \002on channel\002 " + channel);
}

/* Join - Join a channel
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 */
void
UserCommands::Join(ServerConnection *cnx, Person *from,
                   String channel, String rest)
{
  StringTokenizer st(rest);
  channel = st.nextToken();

  if (!Utils::isValidChannelName(channel)) {
    from->sendNotice(String("\002") + channel +
                     " is not a valid channel name\002");
    return;
  }

  // We change the key only if we are not on the channel. We don't trust
  // the user...
  if (!cnx->bot->channelList->getChannel(channel)) {
    if (cnx->bot->wantedChannels[channel])
      cnx->bot->wantedChannels[channel]->key = st.rest();
    else {
      cnx->bot->wantedChannels[channel] = new wantedChannel("", "", st.rest(),
							    0, 0);
    }
  }
  cnx->queue->sendJoin(channel, cnx->bot->wantedChannels[channel]->key);
}

/* Keep - Channel modes to lock
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 */
void
UserCommands::Keep(ServerConnection *cnx, Person *from,
                   String channel, String rest)
{
  String nick = from->getNick();

  if (rest.length() == 0) {
    from->sendNotice("\002No channel modes specified.\002");
    return;
  }

  cnx->bot->channelList->getChannel(channel)->keepModes = rest;
  from->sendNotice(String("\002Keeping modes\002 ") +
                    rest + " \002on channel\002 " +
                    channel);
}

/* Kick - Kick a user
 * Original 17/12/00, Pickle <pickle@alien.net.au>
 */
void UserCommands::Kick(ServerConnection *cnx, Person *from,
			String channel, String rest)
{
   String nick = from->getNick();
   
   Channel * c = cnx->bot->channelList->getChannel(channel);
   
   if (rest.length() == 0) {
      from->sendNotice("Whom dost thou kicketh?");
      return;
   }
   
   StringTokenizer st(rest);
   String who = st.nextToken();
   
   if (who.toLower() == from->getNick().toLower()) {
      from->sendNotice("You might sprain something if you kick yourself.");
      return;
   }
   
   if (Utils::isWildcard(who)) {
      User * u = c->getUser(nick);
      if (!u)
	return;
      if (u->getLevel() < User::TRUSTED_USER) {
	 from->sendNotice("You need an higher level to use wildcards.");
	 return;
      }
   }
   
   if (!cnx->bot->iAmOp(channel)) {
      from->sendNotice(String("I am not a channel op on \002") +
		       channel + String("\002 :("));
      return;
   }
   
   if (Utils::isWildcard(who)) {
      Mask m(who);
      for (map<String, User *, less<String> >::iterator it =
           c->channelMemory.begin();
	   it != c->channelMemory.end();
	   ++it)
	if (m.matches((*it).second->nick + "!" +
		      (*it).second->userhost) &&
	    (*it).second->getProt() < User::NO_KICK)
	  cnx->queue->sendKick(channel, (*it).second->nick, st.rest());
   } else {
      User * u = c->getUser(who);
      if (!u) {
	 from->sendNotice(String("Where is \002") +
			  who + "\002 on \002" + channel + "?\002");
	 return;
      }
      if (u->userListItem->flags & USERFLAG_PROTECT_NOKICK)
	from->sendNotice(String("I don't want to kick \002") +
			 who + "\002 on \002" + channel +
			 "\002 (Protected)");
      else {
	 if (st.rest())
	   cnx->queue->sendKick(channel, who, String("[") + from->getNick() +
				String("] ") + st.rest());
      }
      
   }
}

/* Merge with ban. */
void
UserCommands::KickBan(ServerConnection *cnx, Person *from,
                      String channel, String rest)
{
  StringTokenizer st(rest);

  Ban(cnx, 0, channel, st.nextToken());
  Kick(cnx, from, channel, rest);
}

/* LastSeen - Find out when a user was seen last
 * Original 15/12/00, Pickle <pickle@alien.net.au>
 */
void UserCommands::LastSeen(ServerConnection *cnx, Person *from,
			    String channel, String rest)
  {
     String nick = from->getNick();
     
     if (!rest.length()) {
	from->sendNotice("I'm not a mind reader, who are you asking about?");
	return;
     }

     StringTokenizer st(rest);
     String otherNick = st.nextToken();
     
     if (nick.toLower() == otherNick.toLower()) {
	from->sendNotice("If you want to look at yourself, buy a mirror");
	return;
     }
     
     if (!(otherNick == "-") || 
	 !(Utils::isWildcard(otherNick))) {
	User * u = cnx->bot->channelList->getChannel(channel)->getUser(otherNick);
	UserListItem * uli;
	
	if (u)
	  uli = u->userListItem;
	else 
	  uli = cnx->bot->userList->getUserListItem(otherNick + "!" +
						    cnx->bot->getUserhost(channel,
									  otherNick),
						    channel);
	
	if (!uli) // No user info yet? Maybe they typed the mask?
	  uli = cnx->bot->userList->getUserListItem(otherNick, channel);
	
	if (uli) {
	   if (uli->lastseen > 0) {
	      from->sendNotice(String("\002") + otherNick +
			       String("\002 on \002") + 
			       uli->channelMask.getMask() +
			       String("\002 was last seen \002") +
			       Utils::timelenToStr(cnx->bot->currentTime.time -
						   uli->lastseen) +
			       String("\002 ago."));
	   } else if (uli->lastseen == 0)
		from->sendNotice(String("\002") + otherNick +
				 String("\002 is online now! :)"));
	} else {
	   bool found = false;
	   
	   for (list<UserListItem *>::iterator it = cnx->bot->userList->l.begin();
		it != cnx->bot->userList->l.end();
		it++) 
	     if (((*it)->channelMask.getMask().toLower() == channel.toLower()) &&
		 ((*it)->nicks.toLower() == otherNick.toLower())) {
		from->sendNotice(String("\002") + 
				 Utils::getFirstNick((*it)->nicks) +
				 String("\002 ") +
				 ((Utils::getFirstNick((*it)->nicks).toLower() ==
				   otherNick.toLower()) ?
				  String("") :
				  (String("\"") + otherNick +
				   String("\") "))) +
				 String("on \002") + 
				 (*it)->channelMask.getMask() +
				 String("\002 was last seen \002") +
				 Utils::timelenToStr(cnx->bot->currentTime.time -
						     (*it)->lastseen) +
				 String("\002 ago."));
		
		found = true;
		break;
	     }
	   
	   
	   if (!found)
	     from->sendNotice(String("Sorry, I don't know when \002") + 
			      otherNick + String("\002 was on \002") +
			      channel + String("\002 last."));
	}
     } else { // Run a database search this time...
	int num = 0;
	
	from->sendNotice(String("Last seen userlist search for mask \002") +
			 ((otherNick == "-") ?
			  String("ALL USERS") :
			  otherNick) + 
			 String("\002 on \002") +
			 channel + String("\002:"));
	from->sendNotice("\026 \037Nickname       \037 \037Last seen information                               \037 \026");
	
	for (list<UserListItem *>::iterator it = cnx->bot->userList->l.begin();
	     it != cnx->bot->userList->l.end();
	     it++) {
	   if (((*it)->channelMask.getMask().toLower() == channel.toLower()) &&
	       ((rest == "-") ||
		(rest.toLower() == (*it)->nicks.toLower()))) {
	      num++;
	      from->sendNotice((((*it)->lastseen == 0) ?
				(((*it)->flags & PERSONFLAG_IDENTIFIED) ?
				 String("\002*\002") :
				 String("\002+\002")) : 
				String(" ")) +
			       Utils::getFirstNick((*it)->nicks).prepad(15) +
			       (((*it)->lastseen > 0) ?
				(String(" Seen ") +
				 Utils::timelenToStr(cnx->bot->currentTime.time -
						     (*it)->lastseen) +
				 String(" ago.") + 
				 (((*it)->flags & USERFLAG_LASTSEEN_AUTH) ?
				  String("") : String(" (Unconfirmed)"))) :
				(((*it)->lastseen == 0) ?
				 (String(" Currently online.") +
				  (((*it)->flags & PERSONFLAG_IDENTIFIED) ?
				   String(" (Confirmed)") : String(""))) :
				 String(" No information available."))));
	   }
	   
	}
	
	if (num == 0) 
	  from->sendNotice("\002End of last seen\002 (No matches)");
	else
	  from->sendNotice(String("\002End of last seen\002 (") +
			   String((int)num) + String(" entries)"));
     }
  }

/* Mode - Change a channel mode
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 */
void
  UserCommands::Mode(ServerConnection *cnx, Person *from,
		     String channel, String rest)
{
  String nick = from->getNick();

  if (rest.length() == 0) {
    from->sendNotice("\002I need a parameter for this command.\002");
    return;
  }

  if (!cnx->bot->iAmOp(channel)) {
    from->sendNotice(String("\002I am not channel op on\002 ") +
                     channel);
    return;
  }
                      
  cnx->queue->sendChannelMode(String("MODE ") + channel + " " + rest);
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
		if (length >= 256) {
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
	   if (length >= 256) {
	      from->sendNotice(result);
	      result = ""; length = 0;
	   }
	}
	
	if (result != "")
	  from->sendNotice(result);
     }
     
     from->sendNotice("\002End of names.\002");
  }

/* Merge with Server */
void
UserCommands::NextServer(ServerConnection *cnx, Person *from,
                         String channel, String rest)
{
  if (cnx->bot->serverList->size() == 0) {
    from->sendNotice("\002Server list is empty !\002");
    return;
  }

  if (cnx->bot->serverList->size() == 1) {
    from->sendNotice("\002Server list has only one item. Use"
                     " \"reconnect\" to force reconnection.\002");
    return;
  }

  if (cnx->bot->canChangeServer()) {
    cnx->queue->sendQuit("Changing server");
    cnx->bot->nextServer();
  }
  else
    from->sendNotice("\002I can not change server without"
                     " losing op on a channel I am on.\002");
}

/* Nick - Change nickname
 * Original 14/12/00, Pickle <pickle@alien.net.au>
 */
void
UserCommands::Nick(ServerConnection *cnx, Person *from,
                   String channel, String rest)
{
  StringTokenizer st(rest);
  String nick = st.nextToken();

  if (rest == "") {
    from->sendNotice(String("\002No nickname given.\002"));
    return;
  }

  if (!Utils::isValidNickName(nick)) {
    from->sendNotice(String("\002") + nick +
                     " is not a valid nickname\002");
    return;
  }

  cnx->bot->wantedNickName = nick;
  cnx->queue->sendNick(nick);
}

/* Note - Wrapper into the note interface
 */
void UserCommands::Note(ServerConnection *cnx, Person *from,
			String channel, String rest)
{
   from->sendNotice("The note interface is not operational.");
}

/* NsLookup - Do a name server lookup
 * Original 15/12/00, Pickle <pickle@alien.net.au>
 * Needs: Reverse lookup capabilities and to be ported in having nslookups
 *        via a forked slave portion of the code like TG did
 */
void
UserCommands::NsLookup(ServerConnection *cnx, Person *from,
                       String channel, String rest)
{
  String target;

  if (rest.length() == 0) {
    from->sendNotice("\002You need to supply a nick or an "
                     "host for this command.\002");
    return;
  }

  if (rest.find('.') == -1) {
    StringTokenizer st(cnx->bot->getUserhost("", rest));
    st.nextToken('@');
    target = st.rest();
    if (target.length() == 0) {
      from->sendNotice(String("\002I could not find\002 ") +
                        target);
      return;
    }
  } else
    target = rest;

  struct hostent * host;
  struct in_addr iaddr;

  if (isdigit(target[0])) { // An IP ?
    iaddr.s_addr = inet_addr((const char *)target);
    host = gethostbyaddr((char *)(&iaddr),
                         sizeof(struct in_addr),
                         AF_INET);
    if (host) {
      from->sendNotice(target +
                        " \002is the IP address of\002 " +
                        host->h_name);
      return;
    }
  } else {
    host = gethostbyname((const char *)target);
    if (host) {
      memcpy((char *)&iaddr, (char *)host->h_addr,
            host->h_length);
      from->sendNotice(target + " \002has\002 " +
                        inet_ntoa(iaddr) + " \002for IP address.\002");
      return;
    }
  }

  from->sendNotice(String("\002I could not find host\002 ") +
                    target);
}

/* Op
 * Original 14/12/00, Pickle <pickle@alien.net.au>
 */
void UserCommands::Op(ServerConnection *cnx, Person *from,
		      String channel, String rest)
{
   String nick = from->getNick();
   
   if (!cnx->bot->iAmOp(channel)) {
      from->sendNotice(String("I'm not a channel op on \002") +
		       channel + String("\002 :("));
      return;
   }
   
   if (Utils::isWildcard(rest)) {
      from->sendNotice("Mass op is not allowed. Sorry :(");
      return;
   }
   
   String target;
   
   if (rest.length() == 0)
     target = nick;
   else
     target = rest;
   
   User *u = cnx->bot->channelList->getChannel(channel)->getUser(target);
   if (!u) {
      from->sendNotice(String("I couldn't find \002") + target + 
		       "\002 on \002" + channel + "\002");
      return;
   }
   
   if (!(u->mode & User::OP_MODE))
     cnx->queue->sendChannelMode(channel, "+o", target);
   else {
      if (target == nick)
	from->sendNotice(String("But aren't you already a channel op on \002") 
			 + channel + "\002??!!");
      else
	from->sendNotice(String("But \002") + target + 
			 "\002 is already a channel op on\002 " + channel +
			 "\002!");
   }
}

/* Part - leave a channel
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 */
void UserCommands::Part(ServerConnection *cnx, Person *from,
			String channel, String rest)
{
   wantedChannel *w = cnx->bot->wantedChannels[channel];
   cnx->bot->wantedChannels.erase(channel);
   delete w;
   
   cnx->queue->sendPart(channel, rest);
}

/* Password - change a password
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 * Needs: Fixing.
 */
void
UserCommands::Password(ServerConnection *cnx, Person *from,
                       String channel, String rest)
{
  Channel *c = cnx->bot->channelList->getChannel(channel);

  if (rest.length() == 0) {
    from->sendNotice("\002No password given.\002");
    return;
  }

  User * u = c->getUser(from->getNick());
  if (!u) {
    from->sendNotice(String("\002To change your password for\002") +
                     channel + "\002, you need to be on the "
                     "channel.\002");
    return;
  }

  if (!u->userListItem) {
    from->sendNotice("\002You are not in my userlist.\002");
    return;
  }

  if (rest.toLower() == "none") {
    u->userListItem->passwd = "";
    from->sendNotice("\002Password cleared.\002");
    return;
  }
  
  static char saltChars[] = "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";
  char salt[3];
  srand(cnx->bot->currentTime.time);
  salt[0] = saltChars[rand() % 64];
  salt[1] = saltChars[rand() % 64];
  salt[2] = '\0';

  u->userListItem->passwd = crypt((const char *)rest, salt);
  from->sendNotice("\002Password changed.\002");
}

/* Ping - Ping a user back, tell them the time
 * Original 21/06/01, Pickle <pickle@alien.net.au>
 * Note: This only sends out the ping! The rest of the code is with the PING
 *       reply parser.
 */
void UserCommands::Ping(ServerConnection *cnx, Person *from,
			String channel, String rest)
{
   from->sendCTCP("PING",String(cnx->bot->currentTime.time) + 
		  String(":") + String(cnx->bot->currentTime.millitm));
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

/* Say
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 */
void UserCommands::Say(ServerConnection *cnx, Person *from,
		       String channel, String rest)
{
   if (!rest.length())
     from->sendNotice("And say what?");
   else
     cnx->queue->sendPrivmsg(channel,rest);
}

/* Server
 * Original 14/12/00, Pickle <pickle@alien.net.au>
 * Needs: Join serverlist and nextserver and reconnect into this command
 */
void UserCommands::Server(ServerConnection *cnx, Person *from,
                     String channel, String rest)
{
  int serverNumber;
  String nick = from->getNick();

  if (rest.length() == 0) {
    from->sendNotice("\002You need to supply a server number for this command.\002");
    return;
  }

  serverNumber = atoi(rest);

  if (serverNumber < 0 || serverNumber >= cnx->bot->serverList->size()) {
    from->sendNotice(String((long)serverNumber) +
                      " \002is an invalid server number (see the serverlist).\002");
    return;
  }

  if (cnx->bot->canChangeServer()) {
    cnx->queue->sendQuit("Changing server");
    cnx->queue->flush();
    cnx->bot->connect(serverNumber);
  }
  else
    from->sendNotice("\002I can not change server without losing op on a channel I am on.\002");
}

/* merge with server */
void
UserCommands::ServerList(ServerConnection *cnx, Person *from,
                     String channel, String rest)
{
  String nick = from->getNick();
  
  from->sendNotice("\002Server list:\002");
  long l = 0;
  String s;
  for (vector<class Server *>::iterator it =
         cnx->bot->serverList->v.begin();
       it != cnx->bot->serverList->v.end();
       ++it) {
    s = (*it) == cnx->server ? "\026" : "";
    from->sendNotice(s + String(l++) + s + " " +
                     (*it)->getHostName() +
                     " (" + String((long)((*it)->getPort())) +
                     ")");
  }
  from->sendNotice("\002End of server list.\002");
}

/* Stats - Show a list of statistical mumbo jumbo to impress stupid people
 * Original 19/12/00, Pickle <pickle@alien.net.au>
 */
void UserCommands::Stats(ServerConnection *cnx, Person *from,
			 String channel, String rest)
{
   Version::sendInformation(cnx, from);

   if (rest.length() != 0) {
      from->sendNotice(String("Statistics for \002") + rest +
		       String("\002:"));
      from->sendNotice("This command needs to be finished.");
   } else {
      int num_chans, num_users, num_bots, num_chanowns, num_suspend;
      int num_ident, num_online, num_chanppl, num_chanops, num_chans_on;
      int num_lvluser, num_lvltrusted, num_lvlfriend, num_lvlmaster;
      int num_chanvoiced, num_nopass, peak_chanppl, peak_chanops;
      int peak_chanvoiced, num_dcc_con, num_chanbans, peak_chanbans;
      time_t uptime = cnx->bot->currentTime.time - cnx->bot->startTime;
      
      // This is a scarey way of zeroing every variable!
      num_chans = num_users = num_bots = num_chanowns = num_suspend = 
	num_ident = num_online = num_chanppl, num_chanops = num_chans_on =
	num_lvluser = num_lvltrusted = num_lvlfriend = num_lvlmaster = 
	num_chanvoiced = num_nopass = peak_chanppl = peak_chanops =
	peak_chanvoiced = num_dcc_con = num_chanbans = peak_chanbans = 0;
      
      for (list<UserListItem *>::iterator it = cnx->bot->userList->l.begin();
	   it != cnx->bot->userList->l.end();
	   it++) {
	 num_users++;
	 if ((*it)->flags & PERSONFLAG_IS_BOT)
	   num_bots++;
	 if ((*it)->flags & USERFLAG_IS_CHAN_OWNER)
	   num_chanowns++;
	 if ((*it)->flags & USERFLAG_SUSPENDED)
	   num_suspend++;
	 if ((*it)->flags & PERSONFLAG_IDENTIFIED)
	   num_ident++;
	 if ((*it)->lastseen == 0)
	   num_online++;
	 switch ((*it)->level) {
	  case 1: num_lvluser++;
	    break;
	  case 2: num_lvltrusted++;
	    break;
	  case 3: num_lvlfriend++;
	    break;
	  case 4: num_lvlmaster++;
	    break;
	 }
	 if ((*it)->passwd == "")
	   num_nopass++;
      }
      
      for (map<String, Channel *, less<String> >::iterator it = cnx->bot->channelList->begin(); 
	   it != cnx->bot->channelList->end(); ++it) {
	 Channel *chan = cnx->bot->channelList->getChannel((*it).first);
	 
	 num_chans++;
	 num_chanppl += chan->count;
	 num_chanops += chan->countOp;
	 num_chanvoiced += chan->countVoice;
	 peak_chanppl += chan->count_peak;
	 peak_chanops += chan->countOp_peak;
	 peak_chanvoiced += chan->countVoice_peak;
	 if (cnx->bot->channelList->getChannel((*it).first)->joined)
	   num_chans_on++;
	 for (vector<BanEntry *>::iterator itB = chan->channelBanlist.begin();
	      itB != chan->channelBanlist.end(); ++itB)
	   num_chanbans++;
//	 peak_chanbans += chan->countBans_peak;
      }
      
      for (list<DCCConnection *>::iterator it = cnx->bot->dccConnections.begin();
	   it != cnx->bot->dccConnections.end(); ++it) {
	 num_dcc_con++;
      }
      
      from->sendNotice(String("\002\037General Statistics:\037\002"));
      from->sendNotice(String("Name: ").prepad(10) +
		       String(cnx->bot->nickName + " (" +
			      cnx->bot->wantedNickName + ")").pad(23) +
		       String("  Lag Check: ") +
		       Utils::timeBigToStr(cnx->lag));
#ifdef DEBUG
      from->sendNotice(String("Up time: ").prepad(10) +
		       (!(cnx->bot->debug) ?
			Utils::timelenToStr(uptime) :
			(Utils::timelenToStr(uptime).pad(45) +
			 String("\026 DEBUG MODE \026"))));
#else
      from->sendNotice(String("Up time: ").prepad(10) +
		       Utils::timelenToStr(uptime));
#endif
      from->sendNotice(String("IRC data received: ").prepad(20) +
		       (String((long)(cnx->bot->receivedLen / 1024)) +
			String("k")).pad(15) +
		       String("IRC data sent: ").prepad(20) +
		       (String((long)(cnx->bot->sentLen / 1024)) +
			String("k")));

      from->sendNotice(String("\002\037Number of users:\037 ") + 
		       String(num_users) + String(" (") + 
		       String(num_online) + String(" currently online)\002"));
      from->sendNotice(String("Number of Bots: ").prepad(18) +
		       String(num_bots).pad(6) +
		       String("Channel Admins: ").prepad(18) +
		       String(num_chanowns).pad(6) +
		       String("Suspended: ").prepad(18) +
		       String(num_suspend));
      from->sendNotice(String("Normal Users: ").prepad(18) +
		       String(num_lvluser).pad(6) +
		       String("Trusted Users: ").prepad(18) +
		       String(num_lvltrusted).pad(6) +
		       String("Bot Friends: ").prepad(18) +
		       String(num_lvlfriend));
      from->sendNotice(String("Bot Masters: ").prepad(18) +
		       String(num_lvlmaster).pad(6) +
		       String("Identified Users: ").prepad(18) +
		       String(num_ident).pad(6) +
		       String("Users w/o passwd: ").prepad(18) +
		       String(num_nopass));

      from->sendNotice(String("\002\037Number of channels:\037 ") + 
		       String(num_chans) + String(" (") +
		       String(num_chans_on) + 
		       String(" currently active)\002"));
      from->sendNotice(String("Total People: ").prepad(16) +
		       (String(num_chanppl) + String(" (Peak: ") +
			String(peak_chanppl) + String(")")).pad(19) +
		       String("Total Banned: ").prepad(16) +
		       (String(num_chanbans) + String(" (Peak: ") +
			String(peak_chanbans) + String(")")));
      from->sendNotice(String("Total Opped: ").prepad(16) +
		       (String(num_chanops) + String(" (Peak: ") +
			String(peak_chanops) + String(")")).pad(19) +
		       String("Total Voiced: ").prepad(16) +
		       (String(num_chanvoiced) + String(" (Peak: ") +
			String(peak_chanvoiced) + String(")")));

      from->sendNotice(String("\002\037Number of DCC Fileserver Connections:\037 ") +
		       String(num_dcc_con) + String("\002"));
 
   }
   from->sendNotice(String("\002End of statistics.\002"));
}

/* Raw - Write a raw line to the server
 * Original - 11/12/00, Pickle <pickle@alien.net.au>
 */
void UserCommands::Raw(ServerConnection *cnx, Person *from,
		       String channel, String rest)
  {
     if (rest.length() != 0)
       Commands::Do(cnx->bot, rest);
  }

/* merge with server */
void
UserCommands::Reconnect(ServerConnection *cnx, Person *from,
                        String channel, String rest)
{
  String nick = from->getNick();

  if (cnx->bot->canChangeServer()) {
    cnx->queue->sendQuit("Reconnecting");
    cnx->bot->reconnect();
  }
  else
    from->sendNotice("\002I can not change server without losing op on a channel I am on.\002");
}

/* Test - Test command to try out funky new stuff
 * Do not allow public access!! :)
 */
void UserCommands::Test(ServerConnection *cnx, Person *from,
			String channel, String rest)
  {
     from->sendNotice("\002Begin test dump.\002");
     from->sendNotice(String("channel = ") + String(channel));
     from->sendNotice(String("rest = \002#\002") + String(rest) +
		      String("\002#\002"));

     for (map<String, gameQuizCategory *, less<String> >::iterator it =
	  cnx->bot->games->quiz->gameQuizCategories.begin();
	  it != cnx->bot->games->quiz->gameQuizCategories.end(); ++it) {
	from->sendNotice(String("Quiz dump of \002") +
			 (*it).first + String("\002 (\002") +
			 String(((*it).second)->numQuestions) +
			 String("\002 questions)"));
	for (list<gameQuizQuestion *>::iterator it2 = 
	     ((*it).second)->questions.begin();
	     it2 != ((*it).second)->questions.end(); ++it2) {
	   from->sendNotice(String("Q: ") + (*it2)->question);
	   
	   if ((*it2)->hint.length() > 0) {
	      from->sendNotice(String("H: ") + (*it2)->hint);
	   }
	   
	   for (list<String>::iterator it3 = (*it2)->answers.begin();
		it3 != (*it2)->answers.end(); ++it3) {
//	      from->sendNotice(String("A: ") + (const String &)it3);
	      from->sendNotice(String("A: ") + (*it3));
	   }
	}
     }
       
     from->sendNotice("\002End of test dump.\002");
  }

/* Time - Tell someone the time (duh)
 * Original 6/7/01, Simon Butcher <simonb@alien.net.au>
 */
void UserCommands::Time(ServerConnection *cnx, Person *from,
			String channel, String rest)
 {
    String time = String(ctime(&cnx->bot->currentTime.time));
    
    from->sendNotice(String("The time is \002") +
		     time.subString(0,time.length()-2) +
		     String("\002."));
    
   
   /*
   struct tm *timeNow = localtime(&cnx->bot->currentTime.time);
   
   from->sendNotice(String("The time is \002") +
		    String(timeNow->tm_hour) + String(":") +
		    String(timeNow->tm_min) + String(":") +
		    String(timeNow->tm_sec) + String(".") +
		    String(cnx->bot->currentTime.millitm) + String(" ") +
		    String(cnx->bot->currentTime.timezone) + String(", ") +
		    String(timeNow->tm_wday) + String(" ") +
		    String(timeNow->tm_mday) + String(" ") +
		    String(timeNow->tm_mon) + String(" ") +
		    String(timeNow->tm_year + 1900) +
		    String("\002."));
    */
}

/* Topic - Change the channel's topic
 * Original 15/12/00, Pickle <pickle@alien.net.au>
 */
void
UserCommands::Topic(ServerConnection *cnx, Person *from,
                    String channel, String rest)
{
  String nick = from->getNick();

  if (rest.length() == 0) {
    if (cnx->bot->channelList->getChannel(channel)->channelTopic == "")
      from->sendNotice(String("\002No topic is set for channel\002 ") +
                        channel + "\002.\002");
    else
      from->sendNotice(String("\002Topic for\002 ") +
                        channel + " \002is:\002 " +
                        cnx->bot->channelList->getChannel(channel)->channelTopic);
  } else {
    if (cnx->bot->channelList->getChannel(channel)->lockedTopic)
      from->sendNotice(String("Topic is locked on channel ") +
                       channel);
    else
      cnx->queue->sendTopic(channel, rest);
  }
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
			 Utils::getFirstNick((*it)->nicks).prepad(15) + " " +
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


/* Voice
 * Original 26/06/01, Pickle <pickle@alien.net.au>
 */
void UserCommands::Voice(ServerConnection *cnx, Person *from,
			 String channel, String rest)
{
   String nick = from->getNick();
   
   if (!cnx->bot->iAmOp(channel)) {
      from->sendNotice(String("I'm not a channel op on \002") +
		       channel + String("\002 :("));
      return;
   }
   
   String target;
   
   if (rest.length() == 0)
     target = nick;
   else
     target = rest;
   
   User *u = cnx->bot->channelList->getChannel(channel)->getUser(target);
   if (!u) {
      from->sendNotice(String("I couldn't find \002") + target + 
		       "\002 on \002" + channel + "\002");
      return;
   }
   
   if (!(u->mode & User::VOICE_MODE))
     cnx->queue->sendChannelMode(channel, "+v", target);
   else {
      if (target == nick)
	from->sendNotice(String("Hey, you're already voiced on \002") 
			 + channel + "\002!!");
      else
	from->sendNotice(String("But \002") + target + 
			 "\002 is already voiced on\002 " + channel +
			 "\002!");
   }
}

