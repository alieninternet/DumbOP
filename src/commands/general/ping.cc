
#include "commands.h"
#include "flags.h"
#include "utils.h"

/* Ping - Ping a user back, tell them the time
 * Original 21/06/01, Pickle <pickle@alien.net.au>
 * Note: This only sends out the ping! The rest of the code is with the PING
 *       reply parser.
 */
void Commands::Ping(ServerConnection *cnx, Person *from,
			String channel, String rest)
{
   from->sendCTCP("PING",String(cnx->bot->currentTime.time) + 
		  String(":") + String(cnx->bot->currentTime.millitm));
}
