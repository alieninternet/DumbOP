/* src/irc/commands/connection/say.cc
 * SAY command
 * Copyright (c) 2000 Alien Internet Services
 */

#include "config.h"

#include "commands.h"
#include "utils.h"


/* Say - Say something to a channel
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::Say(Person *from, String channel, String rest)
{
   if (!rest.length()) {
      from->sendLine("And say what?");
   } else {
      from->cnx->queue->sendPrivmsg(channel, rest);
   }
}

