
#include "commands.h"
#include "flags.h"
#include "bot.h"

/* Part - leave a channel
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::Part(ServerConnection *cnx, Person *from,
			String channel, String rest)
{
   wantedChannel *w = cnx->bot->wantedChannels[channel];
   cnx->bot->wantedChannels.erase(channel);
   delete w;
   
   cnx->queue->sendPart(channel, rest);
}

