
#include "commands.h"
#include "utils.h"

/* Raw - Write a raw line to the server
 * Original - 11/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::Raw(ServerConnection *cnx, Person *from,
		       String channel, String rest)
{
   if (rest.length() != 0) {
      cnx->queue->addLine(rest, 0, 0, ServerQueueItem::OTHER);
   }
}

