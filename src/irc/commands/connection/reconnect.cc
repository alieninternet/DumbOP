
#include "config.h"
#include "commands.h"
#include "utils.h"

/* merge with server */
void Commands::Reconnect(Person *from, String channel, String rest)
{
   String nick = from->getNick();
   
   if (from->cnx->bot->canChangeServer()) {
      from->cnx->queue->sendQuit("Reconnecting");
      from->cnx->bot->reconnect();
   } else {
      from->sendLine("\002I can not change server without losing op on a channel I am on.\002");
   }
}

