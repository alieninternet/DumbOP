/* telnetspy.cc
 * Off-relay spy code for debugging
 */

#include <list.h>

#include "telnetspy.h"
#include "telnet.h"
#include "string.h"
#include "stringtokenizer.h"
#include "ansi.h"
#include "utils.h"
#include "bot.h"

void TelnetSpy::spyLine(Bot *bot, String mask, String command, String rest)
{
   StringTokenizer t(rest);
   String to = t.nextToken(' ');
   String line = t.rest();
   
   if (line[0] == ':')
     line = line.subString(1);
   
   for (list<telnetDescriptor *>::iterator it = bot->telnetDaemon->descList.begin();
	it != bot->telnetDaemon->descList.end(); it++)
     if (((*it)->flags & TELNETFLAG_CONNECTED) &&
	 ((*it)->flags & TELNETFLAG_SPYING)) {
	// THIS SHOULD CHANGE! :)
	(*it)->write(IRCtoANSI(mask, command, to, ANSI::toANSI(line)));
     }

}

/* IRCtoANSI - Convert a line from the IRC server into a pretty Ansi line
 * Original 1/1/01, Pickle <pickle@alien.net.au>
 * Notes: Modelled from ircII output with some TG mapping :)
 */
/*#define START_LINE	String(ANSI_NORMAL) + ANSI::scrollRegion(2,23) + \
   ANSI::gotoXY(80, 23) + String("\n") */
#define START_LINE	String(ANSI_NORMAL) + ANSI::scrollRegion(2,23) + \
   ANSI::gotoXY(1, 23) + String("\n")
#define END_LINE	String(ANSI_NORMAL) + ANSI::scrollRegion(25, 25) + \
   ANSI::gotoXY(1, 25)
String TelnetSpy::IRCtoANSI(String mask, String command, String to, 
			    String rest)
{
   String nick;
   String host;
   
   if (mask) {
      StringTokenizer m(mask);
      nick = m.nextToken('!');
      host = m.rest();
   } else {
      nick = mask;
      host = "";
   }
   
   if (command.toUpper() == "JOIN") { // People joining
      return START_LINE + String(ANSI_HEAD_IRC) + nick +
	((host == "") ? String("") :
	 (String(" (") + host + String(")"))) +
	String(" has joined channel ") + to.subString(1) + END_LINE;
   } else if (command == "PART") { // People leaving
      return START_LINE + String(ANSI_HEAD_IRC) + nick +
	String(" has left channel ") + to +
	(((rest == "") || rest == ":") ? String("") :
	 (String(" (") + rest + String(")"))) + END_LINE;
   } else if (command == "QUIT") { // People signing off
      return START_LINE + String(ANSI_HEAD_IRC) + 
	String("Signoff: ") + nick + String(" (") + rest + String(")") + 
	END_LINE;
   } else if (command == "NICK") { // People changing their aliases
      return START_LINE + String(ANSI_HEAD_IRC) + nick +
	String(" is now known as ") + to.subString(1) + END_LINE;
   } else if (command == "PRIVMSG") { // Messages
      if (rest[0] == '\001') { // CTCP request
	 StringTokenizer c(rest);
	 String cCommand = c.nextToken().subString(1);
	 String cQuery = c.rest(); // Should trunc last chr (\001)
	 
	 if (cCommand == "ACTION") { // The /me command
	    if (Utils::isChannel(to)) { // /me to channel
	       return START_LINE + String(ANSI_NGREEN) + String("* ") + 
		 String(ANSI_HGREEN) + nick + String(":") + to +
		 String(ANSI_NGREEN) + String(" ") + 
		 cQuery.subString(0, (cQuery.length() - 2)) + END_LINE;
	    } else { // /me to bot
	       return START_LINE + String(ANSI_NGREEN) + String("* ") + 
		 String(ANSI_HGREEN) + nick + String(ANSI_NGREEN) + 
		 String(" ") + cQuery.subString(0, (cQuery.length() - 2)) + 
		 END_LINE;
	    }
	 }
      } else { // Normal
	 if (Utils::isChannel(to)) { // Message to channel
	    return START_LINE + String(ANSI_NGREEN) + String("<") + 
	      String(ANSI_HWHITE) + nick + String(":") + to +
	      String(ANSI_NGREEN) + String("> ") + String(ANSI_NORMAL) + 
	      rest + END_LINE;
	 } else { // Message to bot
	    return START_LINE + String(ANSI_NGREEN) + String("*") + 
	      String(ANSI_HWHITE) + nick + String(ANSI_NGREEN) + 
	      String("* ") + String(ANSI_NORMAL) + rest + END_LINE;
	 }
      }
   } else if (command == "NOTICE") { // Notices and notifications
      if (rest[0] != '\001') { // CTCP reply
	 if (Utils::isChannel(to)) { // To channel
	    return START_LINE + String(ANSI_NGREEN) + String("-") + 
	      String(ANSI_HWHITE) + nick + String(":") + to + 
	      String(ANSI_NGREEN) + String("- ") + String(ANSI_NORMAL) + 
	      rest + END_LINE;
	 } else { // To bot
	    return START_LINE + String(ANSI_NGREEN) + String("-") + 
	      String(ANSI_HWHITE) + nick + String(ANSI_NGREEN) + 
	      String("- ") + String(ANSI_NORMAL) + rest + END_LINE;
	 }
      }
   } else if (command == "MODE") { // Mode changes
      return START_LINE + String(ANSI_HEAD_IRC) + nick + 
	String(" sets mode: ") + to + String(" ") + rest + END_LINE;
   } else if (command == "TOPIC") { // Topic changes
      return START_LINE + String(ANSI_HEAD_IRC) + nick + 
	String(" changed topic to \"") + rest + String("\"") + 
	END_LINE;
   } else if (command == "KICK") { // Kicks
      StringTokenizer k(rest);
      String kickNick = k.nextToken(' ');
      String reason = k.rest();
	
      if (reason[0] == ':')
	reason = reason.subString(1);
      
      return START_LINE + String(ANSI_HEAD_IRC) + kickNick + 
	String(" was kicked by ") + nick + 
	(((reason == "") || (reason == ":")) ? String("") :
	 (String(" (") + reason + String(")"))) + END_LINE;
   } else if (command == "WALLOPS") { // Wallops broadcast
      return START_LINE + String(ANSI_NRED) + String("!") +
	String(ANSI_HWHITE) + nick + String(ANSI_NRED) +
	String("! ") + String(ANSI_NORMAL) + rest + String(ANSI_HRED) +
	String(ANSI_NORMAL) + END_LINE;
   }

   return String(""); // The line wasn't anything worth displaying.
}

/* spyHeaderInit - Initialise a subheader for the spy session
 * Original 1/1/01, Pickle <pickle@alien.net.au>
 */
String TelnetSpy::spyHeaderInit()
{
   return ANSI::gotoXY(1, 24) + String(ANSI_FINVERSE) + String(ANSI_CLR_LINE) +
     ANSI::scrollRegion(25, 25) + String(ANSI_NORMAL) + ANSI::gotoXY(1, 25) +
     TelnetSpy::spyHeaderUpdate();
}

/* spyHeaderUpdate - Update the subheader for spy sessions
 * Original 1/1/01, Pickle <pickle@alien.net.au>
 */
String TelnetSpy::spyHeaderUpdate()
{
   return String(ANSI_CUR_SAVE) + String(ANSI_FINVERSE) +
     ANSI::gotoXY(1, 24) +
     String(" [DumbOP] [#channel] [flags] [stuff] ") + String((long)time(NULL)) +
     String(ANSI_CUR_RESTORE) + String(ANSI_NORMAL);
}

