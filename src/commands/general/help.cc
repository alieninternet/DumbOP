
#include "commands.h"
#include "flags.h"
#include "utils.h"

/* Help - Display help
 * Original 15/12/00, Pickle <pickle@alien.net.au>
 * 26/12/00 Pickle - Changed header output depending on command or topic
 */
void Commands::Help(ServerConnection *cnx, Person *from,
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
   
   StringTokens st(rest);
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
