
#include "config.h"
#include "commands.h"
#include "utils.h"

/* Say - Say something to a channel
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::Say(ServerConnection *cnx, Person *from,
		       String channel, String rest)
{
   if (!rest.length()) {
      from->sendNotice("And say what?");
   } else {
      cnx->queue->sendPrivmsg(channel, rest);
   }
}

