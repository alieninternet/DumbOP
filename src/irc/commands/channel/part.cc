
#include "config.h"

#include "commands.h"
#include "flags.h"
#include "bot.h"

/* Part - leave a channel
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::Part(Person *from, String channel, String rest)
{
   wantedChannel *w = from->cnx->bot->wantedChannels[channel];
   from->cnx->bot->wantedChannels.erase(channel);
   delete w;
   
   from->cnx->queue->sendPart(channel, rest);
}

