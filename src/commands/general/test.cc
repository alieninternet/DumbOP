
#include "commands.h"
#include "flags.h"
#include "utils.h"

/* Test - Test command to try out funky new stuff
 * Do not allow public access!! :)
 */
void Commands::Test(ServerConnection *cnx, Person *from,
			String channel, String rest)
{
//   from->sendNotice("\002Begin test dump.\002");
//   from->sendNotice(String("channel = ") + String(channel));
//   from->sendNotice(String("rest = \002#\002") + String(rest) +
//		    String("\002#\002"));
   
   // Split out the first word
   StringTokens st(rest);
   String word = st.nextToken();
   
   from->sendNotice(String("\002!\002") + word + String("\002!\002") +
		    Utils::generateSHA1(word) + String("\002!\002"));
}
