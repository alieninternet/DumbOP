
#include "config.h"
#include "commands.h"
#include "utils.h"

/* Do
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 * Needs: Remove command.c dependancy
 */
void Commands::Do(ServerConnection *cnx, Person *from,
		      String channel, String rest)
{
   if (!rest.length()) {
      from->sendNotice("And do what?");
   } else {
      cnx->queue->sendPrivmsg(channel, String("\001ACTION ") + rest +
			      String("\001"));
   }
}

