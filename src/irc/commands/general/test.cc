

#include "config.h"

#include "commands.h"
#include "flags.h"
#include "utils.h"

/* Test - Test command to try out funky new stuff
 * Do not allow public access!! :)
 */
void Commands::Test(Person *from, String channel, String rest)
{
   from->sendLine(String("channel = ") + String(channel));
   from->sendLine(String("rest = \002#\002") + String(rest) + 
		  String("\002#\002"));
   
   from->sendLine(String("I am \"") + from->cnx->bot->wantedNickName + 
		  String("\" <") + from->cnx->bot->userName +
		  String("@") + from->cnx->bot->userHost +
		  String(">"));
   from->sendLine(String("You are ") + from->getNick() + String(", ") +
		  from->getIdent());
   
   // colour lister
   String out = "";
   for (int i = 0; i <= 15; i++) {
      for (int ii = 0; ii <= 15; ii++) {
	 out = out + String("\003") + String(i) + String(",") + String(ii) +
	   String("\002\002") + String(i).prepad(2) + String(",") + String(ii);
      }
      from->sendLine(out);
      out = "";
   }
}
