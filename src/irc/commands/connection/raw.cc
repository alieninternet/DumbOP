/* src/irc/commands/connection/raw.cc
 * RAW command
 * Copyright (c) 2000 Alien Internet Services
 */

#include "config.h"

#include "commands.h"
#include "utils.h"

/* Raw - Write a raw line to the server
 * Original - 11/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::Raw(Person *from, String channel, String rest)
{
   if (rest.length() != 0) {
      from->cnx->queue->addLine(rest, 0, 0, ServerQueueItem::OTHER);
   }
}

