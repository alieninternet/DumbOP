/* src/irc/commands/general/lastseen.cc
 * LASTSEEN command
 * Copyright (c) 1994-2000 Alien Internet Services
 */

#include "config.h"

#include "commands.h"
#include "flags.h"
#include "utils.h"


/* LastSeen - Find out when a user was seen last
 * Original 15/12/00, Pickle <pickle@alien.net.au>
 * Note: Since this was on DumbOP in 1993, this command has appeared everywhere
 */
void Commands::LastSeen(Person *from, String channel, String rest)
{
   String nick = from->getNick();
   
   // Make sure they are asking about someone
   if (!rest.length()) {
      from->sendLine("I'm not a mind reader, who are you asking about?");
      return;
   }
   
   // Grab their nick
   StringTokens st(rest);
   String otherNick = st.nextToken();
   
   // Are they checking up on themselves?
   if (nick.toLower() == otherNick.toLower()) {
      from->sendLine("If you want to look at yourself, buy a mirror");
      return;
   }
   
   // Are they looking at us?
   if (nick.toLower() == from->cnx->bot->nickName.toLower()) {
      from->sendLine("But I'm right here, aren't I?");
      return;
   }
   
   // run through the userlist and find this guy
   for (list<UserListItem *>::iterator it = from->cnx->bot->userList->l.begin();
	it != from->cnx->bot->userList->l.end();
	it++) {
      if ((*it)->nick.toLower() == otherNick.toLower()) {
	 from->sendLine(String("\002") + (*it)->nick + String("\002 ") +
			  (((*it)->lastseen <= 0) ? 
			   (((*it)->lastseen == 0) ?
			    String("is online now!") :
			    String("has no last seen information, sorry.")) :
			   (String("was last seen \002") +
			    Utils::timelenToStr(from->cnx->bot->currentTime.time -
						(*it)->lastseen) +
			    String("\002 ago."))));
	 
	 return;
      }
   }
   
   from->sendLine(String("Sorry, I don't know when \002") + 
		    otherNick + String("\002 was on last."));
}
