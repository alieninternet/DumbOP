/* version.cc
 * Version and Compile information
 */

#include "config.h"
#include "version.h"
#include "person.h"
#include "serverconnection.h"
#include "bot.h"


/* getVersion - Return the version string
 * Original 28/07/01, Simon Butcher <simonb@alien.net.au>
 */
String Version::getVersion(void)
{
   return (String(PROGNAME_STRING) + String(" v") + String(VERNUM_STRING));
}

 
/* getCopyright - Return the copyright string
 * Original 28/07/01, Simon Butcher <simonb@alien.net.au>
 */
String Version::getCopyright(void)
{
#ifdef COMPILE_STRING
   return String(COPYRIGHT_STRING);
#else
   return String("(void)");
#endif
}


/* getBuild - Return the build string
 * Original 28/07/01, Simon Butcher <simonb@alien.net.au>
 */
String Version::getBuild(void)
{
#ifdef BUILD_STRING
   return String(BUILD_STRING);
#else
   retrun String("(void)");
#endif
}


/* getCompile - Return the compile string
 * Original 28/07/01, Simon Butcher <simonb@alien.net.au>
 */
String Version::getCompile(void)
{
   return String(COMPILE_STRING);
}


/* sendInformation - Send information to someone about the software
 * Original 25/11/00, Simon Butcher <simonb@alien.net.au>
 */
void Version::sendInformation(ServerConnection *cnx, Person *from)
{
   from->sendNotice(String("\026 ") + getVersion() + String(" \026 \002") + 
		    getCopyright() + String("\002"));
   from->sendNotice(String("\002Compile Information:\002 ") + getBuild() +
		    String(" (") + getCompile() + String(")"));
   from->sendNotice("Suggestions, comments or questions? E-mail: dumbop@alien.net.au");
}
