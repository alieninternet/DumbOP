
#include "config.h"
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
   
   // colour lister
   String out = "";
   for (int i = 0; i < 15; i++) {
      for (int ii = 0; ii < 15; ii++) {
	 out = out + String("\003") + String(i) + String(",") + String(ii) +
	   String("\002\002") + String(i).prepad(2) + String(",") + String(ii);
      }
      from->sendNotice(out);
      out = "";
   }
}
