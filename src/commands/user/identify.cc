
#include "config.h"

#include <fstream.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ctype.h>
#ifndef _X_OPEN_SOURCE
#define _X_OPEN_SOURCE
#endif
#ifndef _X_OPEN_SOURCE_EXTENDED
#define _X_OPEN_SOURCE_EXTENDED 1
#endif
#ifndef __USE_XOPEN
#define __USE_XOPEN
#endif
#include <unistd.h>
#include <time.h>

#include "commands.h"
#include "flags.h"
#include "utils.h"


/* Ident
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::Identify(ServerConnection *cnx, Person *from,
			 String channel, String rest)
{
   // Is this secure enough to continue?
   if (Utils::isChannel(channel)) {
      from->sendNotice("For security reasons you should not use the IDENTIFY command on a channel.");
      return;
   }

   // Split out the first word
   StringTokens st(rest);
   String word = st.nextToken();
   
   // Make sure we got something
   if (!word.length()) {
      from->sendNotice("You must supply a password to identify yourself!");
      return;
   }
   
   /* Check if we can identify this use properly. Note in requesting the
    * boolean of identification, it also marks this user AS identified as
    * well..
    */
   if (cnx->bot->userList->identify(from->getNick(), word)) {
      // Tell the user they have been identified correctly
      from->sendNotice(String("You have been identified as \002") + 
		       from->getNick() + String("\002."));
   } else {
      // Tell the user they were not identified
      from->sendNotice("Nickname/Password mismatch - You have not been identified.");
   }
}
