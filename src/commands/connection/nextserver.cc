
#include "commands.h"
#include "utils.h"

/* Merge with Server */
void Commands::NextServer(ServerConnection *cnx, Person *from,
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
   } else {
      from->sendNotice("\002I can not change server without"
		       " losing op on a channel I am on.\002");
   }
}

