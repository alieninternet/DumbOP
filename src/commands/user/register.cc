
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


// this should be elsewhere.
#define USER_REGISTRATION_CREDIT	50


/* Register - Add a user, or channel, at somebody's request
 * Original 19/07/01, Simon Butcher <simonb@alien.net.au>
 */
void Commands::Register(ServerConnection *cnx, Person *from,
			    String channel, String rest)
{
   // Is this secure enough to continue?
   if (Utils::isChannel(channel)) {
      from->sendNotice("For security reasons you should not use the REGISTER command on a channel.");
      return;
   }
   
   // Split out the first word
   StringTokens st(rest);
   String word = st.nextToken();
   
   // Make sure we got something
   if (!word.length()) {
      from->sendNotice("You must supply a password!");
      return;
   }
   
   // Are we registering a channel or not?
   if (Utils::isChannel(word)) {
      from->sendNotice(String("You cannot register a channel with this command."));
   } else {
      // Make sure we do not have a duplicate!
      if (cnx->bot->userList->isNickInUserList(from->getNick())) {
	 from->sendNotice(String("Your nickname is already registered!"));
	 return;
      }
      
      // Create the mask. Notice how unrestricted this mask really is...
      String mask = from->getNick() + String("!*@*");

      // Tell the user we are adding them, and add them
      from->sendNotice(String("Registering you as \002") + mask +
		       String("\002 with the password specified, and a registration credit of \002") +
		       String(USER_REGISTRATION_CREDIT) + String("\002."));
      cnx->bot->userList->addUser(mask, cnx->bot->currentTime.time, 
				  User::USER, USER_REGISTRATION_CREDIT, 
				  from->getNick(), 0, 0,
				  Utils::generateSHA1(word.toLower()));
      
      // They have registered, technically they have also identified too.
      from->flags |= PERSONFLAG_IDENTIFIED;
      
      
      
      //////////////////////////////// DIRTY HACK because flags are not 100%
      UserListItem *uli = 
	cnx->bot->userList->getUserListItem(from->getAddress());
      if (uli) {
	 uli->identified = true;
      }
      //////////////////////////////////////////////////////////////////////
   }
}
