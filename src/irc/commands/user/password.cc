
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


/* Password - change a password
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 * Needs: Fixing.
 */
void Commands::Password(Person *from,
			    String channel, String rest)
{
   // Make sure we have their user info...
   if (!from->uli) {
      return;
   }
   
   // Is this secure enough to continue?
   if (Utils::isChannel(channel)) {
      from->sendLine("For security reasons you should not use the PASSWORD command on a channel.");
      return;
   }

   // Split out the first word
   StringTokens st(rest);
   String wordFrom = st.nextToken();
   String wordTo = st.nextToken();
   
   // Make sure we got their original and new passwords
   if (!wordFrom.length() || !wordTo.length()) {
      from->sendLine("You must supply your original password to change it!");
      return;
   }
   
   /* This is the same as identifying someone. Why? It's easier to use the
    * same code, plus it has the benifit of unidentifying them if they are
    * NOT the real person! Remember they have to be identified to use this
    * command in the first place (eg. USER level access at least).
    */
   if (from->cnx->bot->userList->identify(from->getNick(), wordFrom)) {
      // Change the password
      from->uli->passwd = Utils::generateSHA1(wordTo.toLower());
      
      // Tell the user what we have done
      from->sendLine("Password changed.");
   } else {
      /* Tell the user their password was incorrect and now they are no
       * longer identified
       */
      from->sendLine("Nickname/Password mismatch - You are now unidentified.");
   }
}
