
#include "config.h"
#include "commands.h"
#include "utils.h"

/* DelServer
 * Original 18/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::DelServer(ServerConnection *cnx, Person *from,
			     String channel, String rest)
{
   if (rest.length() == 0) {
      from->sendNotice("You need to supply a server number for this command.");
      return;
   }
   
   int serverNumber = atoi(rest);
   
   if ((serverNumber < 0) || (serverNumber >= cnx->bot->serverList->size())) {
      from->sendNotice("Invalid server number.");
      return;
   }
   
   cnx->bot->serverList->delServer(serverNumber);
   
   from->sendNotice(String("Deleted server ") +
		    cnx->bot->serverList->get(serverNumber)->getHostName() +
		    " (" + String((long)cnx->bot->serverList->get(serverNumber)->getPort()) +
		    ").");
}

