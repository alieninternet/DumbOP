
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
	uli = cnx->bot->userList->getUserListItem(from->getAddress(),
						  channel);
      
      if (uli) {
	 from->sendNotice(String("You are \002") +
			  uli->mask.getMask() + "\002 on \002" +
			  uli->channelMask.getMask() + "\002");
	 from->sendNotice(String("Lvl: ") +
			  Utils::levelToStr(uli->level) +
			  " Prot: " +
			  Utils::protToStr(uli->prot) +
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
      
      if (otherNick == "-") {
	 int num = 0, num_on = 0;
	 
	 from->sendNotice(String("Access list search for mask \002") +
			  otherNick + String("\002 on \002") +
			  channel + String("\002:"));
	 from->sendNotice("\026 \037Nickname       \037 \037Level       \037 \037A+o\037 \037A+v\037 \037Right\037 \037Ident\037 \037Flags     \037 \026");
	 
	 for (list<UserListItem *>::iterator it = cnx->bot->userList->l.begin();
	      it != cnx->bot->userList->l.end();
	      it++) {
	    if (((*it)->channelMask.getMask().toLower() == channel.toLower()) &&
		!((*it)->flags & USERFLAG_IS_SUSPENDED)) {
	       from->sendNotice((((*it)->lastseen == 0) ?
				 (((*it)->flags & PERSONFLAG_IDENTIFIED) ?
				  String("\002*\002") :
				  String("\002+\002")) : 
				 String(" ")) +
				Utils::getFirstNick((*it)->nick).prepad(15) + " " +
				Utils::levelToStr((*it)->level).pad(12) + " " +
				(((*it)->flags & USERFLAG_IS_BOT) ?
				 String("Bot") : String("")) +
				Utils::boolToStr((*it)->flags & PERSONFLAG_IDENTIFIED).pad(5) + " " +
				Utils::flagsToStr((*it)->flags).pad(10));
	       num++;
	       if ((*it)->lastseen == 0)
		 num_on++;
	    }
	 }
	 
	 if (num == 0) 
	   from ->sendNotice("\002End of access list\002 (No matches)");
	 else 
	   from->sendNotice(String("\002End of access list\002 (") +
			    String((int)num) + " entries, " + 
			    String((int)num_on) + " online)");
      } else {
	 User * u = cnx->bot->channelList->getChannel(channel)->getUser(otherNick);
	 UserListItem * uli;
	 
	 if (u)
	   uli = u->userListItem;
	 else
	   uli = cnx->bot->userList->getUserListItem(otherNick + "!" +
						     cnx->bot->getUserhost(channel,
									   otherNick),
						     channel);
	 
	 if (uli) {
	    from->sendNotice(String("\002") + otherNick +
			     String("\002 is \002") + uli->mask.getMask() +
			     String("\002 on \002") + uli->channelMask.getMask() + 
			     String("\002"));
	    from->sendNotice(String("Lvl: ") +
			     Utils::levelToStr(uli->level) +
			     " Prot: " +
			     Utils::protToStr(uli->prot) +
			     " Ident: " +
			     Utils::boolToStr(uli && uli->identified));
	 } else {
	    from->sendNotice(String("\002") + otherNick +
			     String("\002 is not in the userlist for \002") +
			     channel + String("\002"));
	 }
      }
   }
}
