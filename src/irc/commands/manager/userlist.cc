/* src/irc/commands/manager/userlist.cc
 * USERLIST command
 * Copyright (c) 1999-2000 Alien Internet Services
 */

#include "config.h"

#include "commands.h"
#include "flags.h"
#include "utils.h"


/* UserList - Display a list of users
 * Original 14/12/00, Pickle <pickle@alien.net.au>
 * 22/12/00 Pickle - Reformatted
 */
void Commands::UserList(Person *from, String channel, String rest)
  {
     int num = 0;
     
     from->sendLine("\002Userlist:\002");
     from->sendLine("\026 \037Nickname       \037 \037Mask            \037 \037Credit    \037 \037Level       \037 \037Flags     \037 \026");
     
     for (list<UserListItem *>::iterator it = from->cnx->bot->userList->l.begin();
	  it != from->cnx->bot->userList->l.end();
	  it++) {
	from->sendLine((((*it)->lastseen == 0) ?
			  (((*it)->identified) ?
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
       from ->sendLine("\002End of userlist\002 (No matches)");
     else 
       from->sendLine(String("\002End of userlist\002 (") +
			String((int)num) + " entries)");
  }

