/* src/irc/commands/general/ping.cc
 * PING command
 * Copyright (c) 2001 Alien Internet Services
 */

#include "config.h"

#include "commands.h"
#include "flags.h"
#include "utils.h"


/* Ping - Ping a user back, tell them the time
 * Original 21/06/01, Pickle <pickle@alien.net.au>
 * Note: This only sends out the ping! The rest of the code is with the PING
 *       reply parser.
 */
void Commands::Ping(Person *from, String channel, String rest)
{
   from->sendCTCP("PING",String(from->cnx->bot->currentTime.time) + 
		  String(":") + String(from->cnx->bot->currentTime.millitm));
}
