
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


/* NsLookup - Do a name server lookup
 * Original 15/12/00, Pickle <pickle@alien.net.au>
 * Needs: Reverse lookup capabilities and to be ported in having nslookups
 *        via a forked slave portion of the code like TG did
 */
void Commands::NsLookup(ServerConnection *cnx, Person *from,
			    String channel, String rest)
{
   String target;
   
   if (rest.length() == 0) {
      from->sendNotice("\002You need to supply a nick or a "
		       "host for this command.\002");
      return;
   }
   
   if (rest.find('.') == -1) {
      StringTokens st(cnx->bot->getUserhost("", rest));
      st.nextToken('@');
      target = st.rest();
      if (target.length() == 0) {
	 from->sendNotice(String("\002I could not find\002 ") +
			  target);
	 return;
      }
   } else
     target = rest;
   
   struct hostent * host;
   struct in_addr iaddr;
   
   if (isdigit(target[0])) { // An IP ?
      iaddr.s_addr = inet_addr((const char *)target);
      host = gethostbyaddr((char *)(&iaddr),
			   sizeof(struct in_addr),
			   AF_INET);
      if (host) {
	 from->sendNotice(target +
			  " \002is the IP address of\002 " +
			  host->h_name);
	 return;
      }
   } else {
      host = gethostbyname((const char *)target);
      if (host) {
	 memcpy((char *)&iaddr, (char *)host->h_addr,
		host->h_length);
	 from->sendNotice(target + " \002has\002 " +
			  inet_ntoa(iaddr) + " \002for IP address.\002");
	 return;
      }
   }
   
   from->sendNotice(String("\002I could not find host\002 ") + target);
}
