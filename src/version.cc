/* version.h
 * Version and Compile information
 */

#include "version.h"
#include "person.h"
#include "serverconnection.h"
#include "bot.h"

void Version::sendInformation(ServerConnection *cnx, Person *from)
{
   from->sendNotice(String("\026") + VERSION_STRING + String("\026"));
   from->sendNotice(String("\002") + COPYRIGHT_STRING + String("\002"));
   from->sendNotice(String("\002Compile Information: ") + COMPILE_STRING +
		    String("\002"));
   from->sendNotice("DumbOP was originally created as a tiny replacement for ChanOP on");
   from->sendNotice("AustNet whenever it went down. DumbOP simply kept the peace for a");
   from->sendNotice("while but is now it is forever growing into something enormous.");
   from->sendNotice(String("Type \002") + String(cnx->bot->commandChar) +
		    "HELP\002 for more information, or \002" +
		    String(cnx->bot->commandChar) +
		    "STATS\002 for statistical junk.");
}
