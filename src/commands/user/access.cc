
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


/* Access
 * Original 20/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::Access(ServerConnection *cnx, Person *from,
			  String channel, String rest)
{
   String nick = from->getNick();
   
   if (!rest.length()) {
      User * u = cnx->bot->channelList->getChannel(channel)->getUser(nick);
      UserListItem * uli;
      
      if (u)
	uli = u->userListItem;
      else
	uli = cnx->bot->userList->getUserListItem(from->getAddress());
      
      if (uli) {
	 from->sendNotice(String("You are \002") +
			  uli->mask.getMask() + "\002, Lvl: " +
			  Utils::levelToStr(uli->level) +
			  " Ident: " +
			  (uli && !uli->identified ? "\026" : "") +
			  Utils::boolToStr(uli && uli->identified) +
			  (uli && !uli->identified ? "\026" : ""));
      } else
	from->sendNotice(String("You are not in the userlist for \002") +
			 channel + "\002");
   } else {
      StringTokens st(rest);
      String otherNick = st.nextToken();
      
      User * u = cnx->bot->channelList->getChannel(channel)->getUser(otherNick);
      UserListItem * uli;
      
      if (u)
	uli = u->userListItem;
      else
	uli = cnx->bot->userList->getUserListItem(otherNick + "!" +
						  cnx->bot->getUserhost(channel,
									otherNick));
      
      if (uli) {
	 from->sendNotice(String("\002") + otherNick +
			  String("\002 is \002") + uli->mask.getMask() +
			  String("\002 Lvl: ") +
			  Utils::levelToStr(uli->level) +
			  " Ident: " +
			  Utils::boolToStr(uli && uli->identified));
      } else {
	 from->sendNotice(String("\002") + otherNick +
			  String("\002 is not in the userlist for \002") +
			  channel + String("\002"));
      }
   }
}
