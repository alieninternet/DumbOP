
#include "commands.h"
#include "flags.h"
#include "utils.h"

/* Save - Force a database save
 * Original 15/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::Save(ServerConnection *cnx, Person *from,
			String channel, String rest)
{
   cnx->bot->userList->save();
   cnx->bot->noteList->save();
   from->sendNotice("Databases saved.");
}
