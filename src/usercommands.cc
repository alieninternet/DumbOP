
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
#include "str.h"
#include "stringtokenizer.h"
#include "flags.h"
#include "sha1.h"

#include "dccconnection.h"

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
				Utils::getFirstNick((*it)->nick).prepad(15) + " " +
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

/* Help - Display help
 * Original 15/12/00, Pickle <pickle@alien.net.au>
 * 26/12/00 Pickle - Changed header output depending on command or topic
 */
void UserCommands::Help(ServerConnection *cnx, Person *from,
			String channel, String rest)
{
   if (rest.length() == 0) {
      int level = Utils::getLevel(cnx->bot, from->getAddress());
      String result = "\002Commands available to you are:\002 ";
      for (list<userFunction *>::iterator it = cnx->bot->userFunctions.begin(); 
	   it != cnx->bot->userFunctions.end(); ++it)
	if ((*it)->minLevel <= level) {
	   result = result + (*it)->name + " ";
	   if (result.length() >= 350) {
	      from->sendNotice(result);
	      result = "";
	   }
	}
      if (result != "")
	from->sendNotice(result);
      from->sendNotice(String("Use \002") + String(cnx->bot->commandChar) +
		       String("HELP <command>\002 for more information (eg. \002") +
		       String(cnx->bot->commandChar) + 
		       String("HELP HELP\002)."));
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
void UserCommands::Identify(ServerConnection *cnx, Person *from,
			 String channel, String rest)
{
}

/* LastSeen - Find out when a user was seen last
 * Original 15/12/00, Pickle <pickle@alien.net.au>
 */
void UserCommands::LastSeen(ServerConnection *cnx, Person *from,
			    String channel, String rest)
{
   String nick = from->getNick();
   
   // Make sure they are asking about someone
   if (!rest.length()) {
      from->sendNotice("I'm not a mind reader, who are you asking about?");
      return;
   }
   
   // Grab their nick
   StringTokenizer st(rest);
   String otherNick = st.nextToken();
   
   // Are they checking up on themselves?
   if (nick.toLower() == otherNick.toLower()) {
      from->sendNotice("If you want to look at yourself, buy a mirror");
      return;
   }
   
   // Are they looking at us?
   if (nick.toLower() == cnx->bot->nickName.toLower()) {
      from->sendNotice("But I'm right here, aren't I?");
      return;
   }
   
   // run through the userlist and find this guy
   for (list<UserListItem *>::iterator it = cnx->bot->userList->l.begin();
	it != cnx->bot->userList->l.end();
	it++) {
      if ((*it)->nick.toLower() == otherNick.toLower()) {
	 from->sendNotice(String("\002") + (*it)->nick + String("\002 ") +
			  (((*it)->lastseen <= 0) ? 
			   (((*it)->lastseen == 0) ?
			    String("is online now!") :
			    String("has no last seen information, sorry.")) :
			   (String("was last seen \002") +
			    Utils::timelenToStr(cnx->bot->currentTime.time -
						(*it)->lastseen) +
			    String("\002 ago."))));
	 
	 return;
      }
   }
   
   from->sendNotice(String("Sorry, I don't know when \002") + 
		    otherNick + String("\002 was on last."));
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

/* Password - change a password
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 * Needs: Fixing.
 */
void
UserCommands::Password(ServerConnection *cnx, Person *from,
                       String channel, String rest)
{
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

/* Register - Add a user, or channel, at somebody's request
 * Original 19/07/01, Simon Butcher <simonb@alien.net.au>
 */
void UserCommands::Register(ServerConnection *cnx, Person *from,
			    String channel, String rest)
{
   // Grab the first word and discard the rest..
   StringTokenizer st(rest);
   String input = st.nextToken().toLower();
     
   from->sendNotice(String("Input: \002!\002") + input + 
		    String("\002!\002 \002!\002") + SHA1::generate(input) + 
		    String("\002!\002)"));
   
   // Are we registering a channel or not?
   if (Utils::isChannel(rest)) {
      from->sendNotice(String("You cannot register a channel with this command."));
   } else {
      // Make sure we do not have a duplicate!
      if (cnx->bot->userList->isNickInUserList(from->getNick())) {
	 from->sendNotice(String("Your nickname is already registered!"));
	 return;
      }
      
      // Create the mask. Notice how unrestricted this mask really is...
      String mask = from->getNick() + String("!*@*");

      // Tell the user we are adding them, and add them
      from->sendNotice(String("Registering you as \002") + mask +
		       String("\002 with the password specified."));
      cnx->bot->userList->addUser(mask, "#*", User::USER, 0, 
				  from->getNick(), 0, 0, 
				  SHA1::generate(input));
   }
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
	  cnx->bot->games->quiz->categories.begin();
	  it != cnx->bot->games->quiz->categories.end(); ++it) {
	if ((*it).second) {
	   from->sendNotice(String("Quiz dump of \002") +
			    (*it).first + String("\002 (\002") +
			    String(((*it).second)->numQuestions) +
			    String("\002 questions)"));
	   for (vector<gameQuizQuestion *>::iterator it2 = 
		((*it).second)->questions.begin();
		it2 != ((*it).second)->questions.end(); ++it2) {
	      if (*it2) {
		 from->sendNotice(String("Q: ") + (*it2)->question);
		 
		 if ((*it2)->hint.length() > 0) {
		    from->sendNotice(String("H: ") + (*it2)->hint);
		 }
		 
		 for (list<String>::iterator it3 = (*it2)->answers.begin();
		      it3 != (*it2)->answers.end(); ++it3) {
		    if (*it3) {
		       from->sendNotice(String("A: ") + (*it3));
		    }
		 }
	      }
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
   struct tm *timeNow = localtime(&cnx->bot->currentTime.time);
   
   from->sendNotice(String("The time is \002") +
		    String(timeNow->tm_hour).prepad(2, '0') + String(":") +
		    String(timeNow->tm_min).prepad(2, '0') + String(":") +
		    String(timeNow->tm_sec).prepad(2, '0') + String(".") +
		    String(cnx->bot->currentTime.millitm).prepad(3, '0') + 
		    String("\002, \002") + 
		    Utils::intToDayOfWeek(timeNow->tm_wday) + String(" ") + 
		    String(timeNow->tm_mday) + String(" ") +
		    Utils::intToMonth(timeNow->tm_mon) + String(" ") +
		    String(timeNow->tm_year + 1900) + String("\002."));
}

