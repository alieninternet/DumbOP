
#include "commands.h"
#include "utils.h"

/* merge with server */
void Commands::Reconnect(ServerConnection *cnx, Person *from,
			     String channel, String rest)
{
   String nick = from->getNick();
   
   if (cnx->bot->canChangeServer()) {
      cnx->queue->sendQuit("Reconnecting");
      cnx->bot->reconnect();
   } else {
      from->sendNotice("\002I can not change server without losing op on a channel I am on.\002");
   }
}

