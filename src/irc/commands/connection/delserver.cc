/* src/irc/commands/connection/delserver.cc
 * DELSERVER command
 * Copyright (c) 2000 Alien Internet Services
 */

#include "config.h"

#include "commands.h"
#include "utils.h"

/* DelServer
 * Original 18/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::DelServer(Person *from, String channel, String rest)
{
   if (rest.length() == 0) {
      from->sendLine("You need to supply a server number for this command.");
      return;
   }
   
   int serverNumber = atoi(rest);
   
   if ((serverNumber < 0) || (serverNumber >= from->cnx->bot->serverList->size())) {
      from->sendLine("Invalid server number.");
      return;
   }
   
   from->cnx->bot->serverList->delServer(serverNumber);
   
   from->sendLine(String("Deleted server ") +
		    from->cnx->bot->serverList->get(serverNumber)->getHostName() +
		    " (" + String((long)from->cnx->bot->serverList->get(serverNumber)->getPort()) +
		    ").");
}

