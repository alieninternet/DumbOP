/* src/irc/commands/connection/do.cc
 * DO command
 * Copyright (c) 2000 Alien Internet Services
 */

#include "config.h"

#include "commands.h"
#include "utils.h"

/* Do
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 * Needs: Remove command.c dependancy
 */
void Commands::Do(Person *from, String channel, String rest)
{
   if (!rest.length()) {
      from->sendLine("And do what?");
   } else {
      from->cnx->queue->sendPrivmsg(channel, String("\001ACTION ") + rest +
			      String("\001"));
   }
}

