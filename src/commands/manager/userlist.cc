
#include "commands.h"
#include "flags.h"
#include "utils.h"

/* UserList - Display a list of users
 * Original 14/12/00, Pickle <pickle@alien.net.au>
 * 22/12/00 Pickle - Reformatted
 */
void Commands::UserList(ServerConnection *cnx, Person *from,
			    String channel, String rest)
  {
     int num = 0;
     
     from->sendNotice("\002Userlist:\002");
     from->sendNotice("\026 \037Nickname       \037 \037Mask            \037 \037Credit    \037 \037Level       \037 \037Flags     \037 \026");
     
     for (list<UserListItem *>::iterator it = cnx->bot->userList->l.begin();
	  it != cnx->bot->userList->l.end();
	  it++) {
	from->sendNotice((((*it)->lastseen == 0) ?
			  (((*it)->flags & PERSONFLAG_IDENTIFIED) ?
			   String("\002*\002") :
			   String("\002+\002")) : 
			  String(" ")) +
			 Utils::getFirstNick((*it)->nick).prepad(15) + " " +
			 String("\002[\002") +
			 (*it)->mask.getMask().pad(14) + String("\002]\002 ") +
			 String((*it)->credits).pad(10) + " " +
			 Utils::levelToStr((*it)->level).pad(12) + " " +
			 Utils::flagsToStr((*it)->flags).pad(10) + " " +
			 String((*it)->flags));
	num++;
     }
     
     if (num == 0) 
       from ->sendNotice("\002End of userlist\002 (No matches)");
     else 
       from->sendNotice(String("\002End of userlist\002 (") +
			String((int)num) + " entries)");
  }

