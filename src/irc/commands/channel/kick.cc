/* src/irc/commands/channel/kick.cc
 * KICK command
 * Copyright (c) 2000 Alien Internet Services
 */

#include "config.h"

#include "commands.h"
#include "flags.h"
#include "bot.h"
#include "utils.h"


/* Kick - Kick a user
 * Original 17/12/00, Simon Butcher <simonb@alien.net.au>
 */
void Commands::Kick(Person *from, String channel, String rest)
{
   String nick = from->getNick();
   
   Channel * c = from->cnx->bot->channelList->getChannel(channel);
   
   if (rest.length() == 0) {
      from->sendLine("Whom dost thou kicketh?");
      return;
   }
   
   StringTokens st(rest);
   String who = st.nextToken();
   
   if (who.toLower() == from->getNick().toLower()) {
      from->sendLine("You might sprain something if you kick yourself.");
      return;
   }
   
   if (Utils::isWildcard(who)) {
      User * u = c->getUser(nick);
      if (!u)
	return;
      if (u->getLevel() < User::TRUSTED_USER) {
	 from->sendLine("You need an higher level to use wildcards.");
	 return;
      }
   }
   
   if (!from->cnx->bot->iAmOp(channel)) {
      from->sendLine(String("I am not a channel op on \002") +
		       channel + String("\002 :("));
      return;
   }
   
   if (Utils::isWildcard(who)) {
      Mask m(who);
      for (map<String, User *, less<String> >::iterator it =
           c->channelMemory.begin();
	   it != c->channelMemory.end();
	   ++it)
	if (m.matches((*it).second->nick + "!" +
		      (*it).second->userhost) &&
	    (*it).second->getProt() < User::NO_KICK)
	  from->cnx->queue->sendKick(channel, (*it).second->nick, st.rest());
   } else {
      User * u = c->getUser(who);
      if (!u) {
	 from->sendLine(String("Where is \002") +
			  who + "\002 on \002" + channel + "?\002");
	 return;
      }
      if (st.rest().length())
	from->cnx->queue->sendKick(channel, who, String("[") + 
				   from->getNick() +
				   String("] ") + st.rest());
      
   }
}

