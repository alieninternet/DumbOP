
#include "config.h"
#include "commands.h"
#include "utils.h"

/* Server
 * Original 14/12/00, Pickle <pickle@alien.net.au>
 * Needs: Join serverlist and nextserver and reconnect into this command
 */
void Commands::Server(ServerConnection *cnx, Person *from,
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
   } else {
      from->sendNotice("\002I can not change server without losing op on a channel I am on.\002");
   }
}

