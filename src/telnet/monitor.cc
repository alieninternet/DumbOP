/* src/telnet/monitor.cc
 * Off-relay monitoring, an integral part of the MONITOR dialogue
 */

#include "config.h"

#include <list.h>

#include "telnet.h"
#include "telnetdialogue.h"
#include "ansi.h"
#include "utils.h"


/* monitorLine - Send a freshly parsed line to any monitoring terminals
 * Original 1/1/01, Pickle <pickle@alien.net.au>
 * Note: Yes, this is how I spent my lifeless millenium new-years morning.
 *       Party. Woo. Yay. :-/
 */
void Telnet::monitorLine(String mask, String command, String rest)
{
   StringTokens t(rest);
   String to = t.nextToken(' ');
   String line = t.rest();

   // If the line is a multi-worded line, rip away the little marker guy
   if (line[0] == ':') {
      line = line.subString(1);
   }

   // Convert the line once
   String convertedLine = IRCtoANSI(mask, command, to, line);
   
   // Should we bother? (eg. converted sent us anything?)
   if (!convertedLine.length()) {
      return;
   }
   
   // Run through the descriptors...
   for (list<TelnetDescriptor *>::iterator it = descList.begin();
	it != descList.end(); it++) {
      // Is this descriptor monitoring?
      if (((*it)->flags & TELNETFLAG_CONNECTED) &&
	  ((*it)->dialogue->type == TelnetDialogue::MONITOR)) {
	 // this really should change sometime...... masking??
	 (*it)->write(String(ANSI_CUR_SAVE) + String(ANSI_NORMAL) +
		      ANSI::gotoXY((*it)->columns, ((*it)->rows - 2)) +
		      String("\r\n") + 
		      ANSI::IRCtoANSI(convertedLine, (*it)->columns) +
		      String(ANSI_CUR_RESTORE));
      }
   }
}


/* IRCtoANSI - Convert a line from the IRC server into a pretty Ansi line
 * Original 1/1/01, Pickle <pickle@alien.net.au>
 * Note: Modelled from ircII output with style a'la 'The Gathering' :)
 */
String Telnet::IRCtoANSI(String mask, String command, String to, 
			 String rest)
{
   String nick;
   String host;
   
   // Dodgey thing to get the time, since I am too lazy to link Bot*...
   time_t currentTime = time(NULL);
   struct tm *timeNow = localtime(&currentTime);
   
   /* Start the line off with the usual ANSI cursor relocation stuff to
    * bump down a line in the scroll box. We also append a time sequence
    * code here, hopefully ppl won't mind :) Notice how we set NORMAL
    * prior to moving the cursor, then DIM afterwards. Why not reset then
    * dim? This seems to induce ANSI crud (bleeding) sometimes on a lot of
    * terminals (shows how well some people follow standards *COUGH*)
    */
   String line = (String(ANSI_DIM) + 
		  String("[") + String(timeNow->tm_hour).prepad(2, '0') +
		  String(":") + String(timeNow->tm_min).prepad(2, '0') +
		  String(":") + String(timeNow->tm_sec).prepad(2, '0') +
		  String("] ") + String(ANSI_NORMAL));
   
   if (mask.length()) {
      StringTokens m(mask);
      nick = m.nextToken('!');
      host = m.rest();
   } else {
      nick = mask;
      host = "";
   }
   
   if (command == "JOIN") { // People joining
      line = line + (String(ANSI_HEAD_IRC) + nick +
		     ((host == "") ? String("") :
		      (String(" (") + host + String(")"))) +
		     String(" has joined channel ") + to.subString(1));
   } else if (command == "PART") { // People leaving
      line = line + (String(ANSI_HEAD_IRC) + nick +
		     String(" has left channel ") + to +
		     ((rest == "") ? String("") :
		      (String(" (") + rest + String(")"))));
   } else if (command == "QUIT") { // People signing off
      line = line + (String(ANSI_HEAD_IRC) + String("Signoff: ") + nick + 
		     String(" (") + to.subString(1) + String(" ") + rest + 
		     String(")"));
   } else if (command == "NICK") { // People changing their aliases
      line = line + (String(ANSI_HEAD_IRC) + nick + 
		     String(" is now known as ") + to.subString(1));
   } else if (command == "PRIVMSG") { // Messages
      if (rest[0] == '\001') { // CTCP request
	 StringTokens c(rest);
	 String cCommand = c.nextToken().subString(1);
	 String cQuery = c.rest(); // Should trunc last chr (\001)
	 
	 if (cCommand == "ACTION") { // The /me command
	    if (Utils::isChannel(to)) { // /me to channel
	       line = line + (String(ANSI_NGREEN) + String("* ") + 
			      String(ANSI_HGREEN) + nick + String(":") + to +
			      String(ANSI_NGREEN) + String(" ") + 
			      cQuery.subString(0, (cQuery.length() - 2)));
	    } else { // /me to bot
	       line = line + (String(ANSI_NGREEN) + String("* ") + 
			      String(ANSI_HGREEN) + nick + 
			      String(ANSI_NGREEN) + String(" ") + 
			      cQuery.subString(0, (cQuery.length() - 2)));
	    }
	 } else { // We do not show CTCP requests
	    return "";
	 }
      } else { // Normal
	 if (Utils::isChannel(to)) { // Message to channel
	    line = line + (String(ANSI_NGREEN) + String("<") + 
			   String(ANSI_HWHITE) + nick + String(":") + to +
			   String(ANSI_NGREEN) + String("> ") + 
			   String(ANSI_NORMAL) + rest);
	 } else { // Message to bot
	    line = line + (String(ANSI_NGREEN) + String("*") +
			   String(ANSI_HWHITE) + nick + String(ANSI_NGREEN) + 
			   String("* ") + String(ANSI_NORMAL) + rest);
	 }
      }
   } else if (command == "NOTICE") { // Notices and notifications
      if (rest[0] != '\001') { // CTCP reply
	 if (nick.length()) { // From real source (server/user?)
	    if (Utils::isChannel(to)) { // To channel
	       line = line + (String(ANSI_NGREEN) + String("-") + 
			      String(ANSI_HWHITE) + nick + String(":") + to + 
			      String(ANSI_NGREEN) + String("- ") + 
			      String(ANSI_NORMAL) + rest);
	    } else { // To bot
	       line = line + (String(ANSI_NGREEN) + String("-") + 
			      String(ANSI_HWHITE) + nick + String(ANSI_NGREEN) + 
			      String("- ") + String(ANSI_NORMAL) + rest);
	    }
	 } else {
	    /* Line is from an obscure source 
	     * (eg. server generated, not tagged)
	     */
	    line = line + (String(ANSI_HEAD_IRC) + rest);
	 }
      } else { // We do not show CTCP replies
	 return "";
      }
   } else if (command == "MODE") { // Mode changes
      line = line + (String(ANSI_HEAD_IRC) + nick + String(" sets mode: ") + 
		     to + String(" ") + rest);
   } else if (command == "TOPIC") { // Topic changes
      line = line + (String(ANSI_HEAD_IRC) + nick + 
		     String(" changed the topic in ") + to +
		     String(" to \"") + rest + String("\""));
   } else if (command == "KICK") { // Kicks
      StringTokens k(rest);
      String kickNick = k.nextToken(' ');
      String reason = k.rest();
	
      if (reason[0] == ':') {
	 reason = reason.subString(1);
      }
      
      line = line + (String(ANSI_HEAD_IRC) + kickNick +
		     String(" was kicked from ") + to + 
		     String(" by ") + nick + 
		     (((reason == "") || (reason == ":")) ? String("") :
		      (String(" (") + reason + String(")"))));
   } else if (command == "WALLOPS") { // Wallops broadcast
      line = line + (String(ANSI_NRED) + String("!") +
		     String(ANSI_HWHITE) + nick + String(ANSI_NRED) +
		     String("! ") + String(ANSI_NORMAL) + to.subString(1) + 
		     String(" ") + rest + String(ANSI_HRED) + 
		     String(ANSI_NORMAL));
   } else {
      return ""; /* The line wasn't anything worth displaying. */
   }
   
   // Reset the ANSI code at the end
   line = line + String(ANSI_NORMAL);

   return line;
}

