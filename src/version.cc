/* version.cc
 * Version and Compile information
 */

#include "version.h"
#include "person.h"
#include "serverconnection.h"
#include "bot.h"

void Version::sendInformation(ServerConnection *cnx, Person *from)
{
   from->sendNotice(String("\026 ") + VERSION_STRING + 
		    String(" \026 \002") + COPYRIGHT_STRING + String("\002"));
   from->sendNotice(String("\002Compile Information:\002 ") + BUILD_STRING +
		    String(" (") + COMPILE_STRING + String(")"));
   from->sendNotice("Suggestions, comments or questions? E-mail: dumbop@alien.net.au");
}
