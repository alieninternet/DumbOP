
#include "commands.h"
#include "flags.h"
#include "bot.h"

/* Rules - Display channel rules
 * Original 18/07/01, Simon Butcher <simonb@alien.net.au>
 */
void Commands::Rules(ServerConnection *cnx, Person *from,
		       String channel, String rest)
{
   // Grab the channel info
   Channel *c = cnx->bot->channelList->getChannel(channel);
   
   // Check if there are rules we need to list..
   if (!cnx->bot->wantedChannels[c->channelName]->rules.length()) {
      // No rules - no comment
      return;
   }
   
   // Otherwise, return the rules.
   from->sendNotice(String("\002") + c->channelName + String(" rules:\002 ") +
		    cnx->bot->wantedChannels[c->channelName]->rules);
}
