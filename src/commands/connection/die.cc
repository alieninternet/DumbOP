
#include "config.h"
#include "commands.h"
#include "utils.h"

/* Die - Make the bot quit
 * Original 13/12/00, Pickle <pickle@alien.net.au>
 * 28/12/00 Pickle - Changed default quit message to Version::getVersion()
 */
void Commands::Die(ServerConnection *cnx, Person *from,
		       String channel, String rest)
{
   // Quit IRC
   if (rest.length() == 0) {
      cnx->queue->sendQuit(from->getNick() + 
			   String(" requested me to leave [") + 
			   Version::getVersion() + String("]"));
   } else {
      cnx->queue->sendQuit(rest);
   }
   
   // Tell the bot to stop now
   cnx->bot->stop = true;
}

