
#include "config.h"
#include "commands.h"
#include "flags.h"
#include "bot.h"

/* Cycle
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::Cycle(ServerConnection *cnx, Person *from,
			 String channel, String rest)
{
   // Leave..
   if (rest.length()) {
      cnx->queue->sendPart(channel, String("[Cycle] ") + rest);
   } else {
      cnx->queue->sendPart(channel, String("Cycle"));
   }
   
   // Join..
   cnx->queue->sendJoin(channel, cnx->bot->wantedChannels[channel]->key);
   
   // Explain..
   from->sendNotice(String("Cycled on channel \002") + channel + 
		    String("\002"));
}

