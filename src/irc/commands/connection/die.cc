/* src/irc/commands/connection/die.cc
 * DIE command
 * Copyright (c) 1998-2000 Alien Internet Services
 */

#include "config.h"

#include "commands.h"
#include "utils.h"

/* Die - Make the bot quit
 * Original 13/12/00, Pickle <pickle@alien.net.au>
 * 28/12/00 Pickle - Changed default quit message to Version::getVersion()
 */
void Commands::Die(Person *from, String channel, String rest)
{
   // Quit IRC
   if (rest.length() == 0) {
      from->cnx->queue->sendQuit(from->getNick() + 
			   String(" requested me to leave [") + 
			   Version::getVersion() + String("]"));
   } else {
      from->cnx->queue->sendQuit(rest);
   }
   
   // Tell the bot to stop now
   from->cnx->bot->stop = true;
}

