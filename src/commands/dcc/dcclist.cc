
#include "config.h"
#include "commands.h"
#include "dccconnection.h"


/* DCCList
 * Original 19/12/00, Pickle <pickle@alien.net.au>
 */
void
Commands::DCCList(ServerConnection *cnx, Person *from,
                      String channel, String rest)
{
  from->sendNotice("\002DCClist:\002");
  from->sendNotice("\002Hostname                         Last used\002");

  for (list<DCCConnection *>::iterator it =
         cnx->bot->dccConnections.begin();
       it != cnx->bot->dccConnections.end();
       ++it) {
    from->sendNotice((*it)->nuh.pad(32) + " " +
                     String((long)(cnx->bot->currentTime.time -
                                   (*it)->lastSpoken)));
  }

  from->sendNotice("\002End of dcclist.\002");
}

