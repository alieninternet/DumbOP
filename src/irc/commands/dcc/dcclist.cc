
#include "config.h"
#include "commands.h"
#include "dccconnection.h"


/* DCCList
 * Original 19/12/00, Pickle <pickle@alien.net.au>
 */
void
Commands::DCCList( Person *from,
                      String channel, String rest)
{
  from->sendLine("\002DCClist:\002");
  from->sendLine("\002Hostname                         Last used\002");

  for (list<DCCConnection *>::iterator it =
         from->cnx->bot->dccConnections.begin();
       it != from->cnx->bot->dccConnections.end();
       ++it) {
    from->sendLine((*it)->nuh.pad(32) + " " +
                     String((long)(from->cnx->bot->currentTime.time -
                                   (*it)->lastSpoken)));
  }

  from->sendLine("\002End of dcclist.\002");
}

