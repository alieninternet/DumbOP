
#include "config.h"
#include "commands.h"
#include "utils.h"

/* Merge with Server */
void Commands::NextServer(Person *from, String channel, String rest)
{
   if (from->cnx->bot->serverList->size() == 0) {
      from->sendLine("\002Server list is empty !\002");
      return;
   }
   
   if (from->cnx->bot->serverList->size() == 1) {
      from->sendLine("\002Server list has only one item. Use"
		       " \"reconnect\" to force reconnection.\002");
      return;
   }
   
   if (from->cnx->bot->canChangeServer()) {
      from->cnx->queue->sendQuit("Changing server");
      from->cnx->bot->nextServer();
   } else {
      from->sendLine("\002I can not change server without"
		       " losing op on a channel I am on.\002");
   }
}

