
#include "config.h"

#include "commands.h"
#include "flags.h"
#include "bot.h"

/* Cycle
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::Cycle(Person *from, String channel, String rest)
{
   // Leave..
   if (rest.length()) {
      from->cnx->queue->sendPart(channel, String("[Cycle] ") + rest);
   } else {
      from->cnx->queue->sendPart(channel, String("Cycle"));
   }
   
   // Join..
   from->cnx->queue->sendJoin(channel, 
			      from->cnx->bot->wantedChannels[channel]->key);
   
   // Explain..
   from->sendLine(String("Cycled on channel \002") + channel + 
		    String("\002"));
}

