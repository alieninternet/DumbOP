/* src/irc/commands/channel/rules.cc
 * RULES command
 * Copyright (c) 2001 Alien Internet Services
 */

#include "config.h"

#include "commands.h"
#include "flags.h"
#include "bot.h"

/* Rules - Display channel rules
 * Original 18/07/01, Simon Butcher <simonb@alien.net.au>
 */
void Commands::Rules(Person *from, String channel, String rest)
{
   // Grab the channel info
   Channel *c = from->cnx->bot->channelList->getChannel(channel);
   
   // Check if there are rules we need to list..
   if (!from->cnx->bot->wantedChannels[c->channelName]->rules.length()) {
      // No rules - no comment
      return;
   }
   
   // Otherwise, return the rules.
   from->sendLine(String("\002") + c->channelName + String(" rules:\002 ") +
		    from->cnx->bot->wantedChannels[c->channelName]->rules);
}
