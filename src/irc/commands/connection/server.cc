/* src/irc/commands/connection/server.cc
 * SERVER command
 * Copyright (c) 1997-2000 Alien Internet Services
 */

#include "config.h"

#include "commands.h"
#include "utils.h"

/* Server
 * Original 14/12/00, Pickle <pickle@alien.net.au>
 * Needs: Join serverlist and nextserver and reconnect into this command
 */
void Commands::Server(Person *from, String channel, String rest)
{
   int serverNumber;
   String nick = from->getNick();
   
   if (rest.length() == 0) {
      from->sendLine("\002You need to supply a server number for this command.\002");
      return;
   }
   
   serverNumber = atoi(rest);
   
   if (serverNumber < 0 || serverNumber >= from->cnx->bot->serverList->size()) {
      from->sendLine(String((long)serverNumber) +
		       " \002is an invalid server number (see the serverlist).\002");
      return;
   }
   
   if (from->cnx->bot->canChangeServer()) {
      from->cnx->queue->sendQuit("Changing server");
      from->cnx->queue->flush();
      from->cnx->bot->connect(serverNumber);
   } else {
      from->sendLine("\002I can not change server without losing op on a channel I am on.\002");
   }
}

