/* src/irc/commands/general/help.cc
 * HELP command
 * Copyright (c) 1997-2001 Alien Internet Services
 */

#include "config.h"

#include "commands.h"
#include "flags.h"
#include "utils.h"


/* Help - Display help
 * Original 15/12/00, Simon Butcher <simonb@alien.net.au>
 * 26/12/00 simonb - Changed header output depending on command or topic
 * 01/08/01 simonb - Commands not available to the user are now blocked
 */
void Commands::Help(Person *from, String channel, String rest)
{
   // Grab the user level for this person
   int level = Utils::getLevel(from->cnx->bot, from->getAddress());

   // Are we making up 'generic' help?
   if (rest.length() == 0) {
      String result = "\002Commands available to you are:\002 ";
      
      for (list<userFunction *>::iterator it = from->cnx->bot->userFunctions.begin(); 
	   it != from->cnx->bot->userFunctions.end(); ++it) {
	 if (((*it)->minLevel <= level) && !((*it)->hidden)) {
	    result = result + (*it)->name + " ";

	    // Check if the result length is getting a little long
	    if (result.length() >= 350) {
	       from->sendLine(result);
	       result = "";
	    }
	 }
      }

      // Send the last result responce, if any
      if (result != "") {
	from->sendLine(result);
      }
      
      from->sendLine(String("Use \002") + String(from->cnx->bot->commandChar) +
		     String("HELP <command>\002 for more information (eg. \002") +
		     String(from->cnx->bot->commandChar) + 
		     String("HELP HELP\002)."));
      return;
   }
   
   StringTokens st(rest);
   String command = st.nextToken().toUpper();
   ifstream helpFile(from->cnx->bot->helpFileName);
   
   // Check that the help file is alive
   if (!helpFile) {
      from->sendLine("I can't find the help file, sorry.");
      return;
   }
   
   String buf;

   // Run through the helpfile and find this help topic
   while (!helpFile.eof()) {
      helpFile >> buf;

      if (buf.subString(0, command.length()) == String(":") + command) {
	 buf = buf.subString(1);
	 int cmdLevel = -1;
	 
	 // Try and find this command in the user function list...
	 for (list<userFunction *>::iterator it = 
	      from->cnx->bot->userFunctions.begin(); 
	      it != from->cnx->bot->userFunctions.end(); ++it) {
	    if ((*it)->name == command) {
	       cmdLevel = (*it)->minLevel;
	       break;
	    }
	 }
	 
	 // If we found the command, we are able to mention it in the output
	 if (cmdLevel > -1) {
	    // Does this user have access to this command?
	    if (level >= cmdLevel) {
	       from->sendLine(String(String("Help for command \002") +
				       String(from->cnx->bot->commandChar) +
				       command + String("\002:")).pad(40) + 
				String("Access Required: \002") +
				Utils::levelToStr(cmdLevel) +
				String("\002"));
	    } else {
	       // They don't have access to this command. Say we didn't find it
	       from->sendLine(String("Couldn't find \002") +
				command + "\002 in the helpfile.");
	       return;
	    }
	 } else {
	    from->sendLine(String(String("Help on topic \002\"") + command +
				    String("\002\":")));
	 }
	    
	 while (buf != "") {
	    from->sendLine(buf);
	    helpFile >> buf;
	 }
	 
	 from->sendLine("\002End of help.\002");
	 return;
      }
   }

   // If we got here, that entry was not in the helpfile
   from->sendLine(String("Couldn't find \002") +
		    command + "\002 in the helpfile.");
}
