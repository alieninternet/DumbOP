/* src/irc/commands/channel/op.cc
 * OP command
 * Copyright (c) 2000 Alien Internet Services
 */

#include "config.h"

#include "commands.h"
#include "flags.h"
#include "bot.h"
#include "utils.h"

/* Op - Give a user +o (channel operator) status
 * Original 14/12/00, Simon Butcher <simonb@alien.net.au>
 */
void Commands::Op(Person *from, String channel, String rest)
{
   String nick = from->getNick();
   
   if (!from->cnx->bot->iAmOp(channel)) {
      from->sendLine(String("I'm not a channel op on \002") +
		       channel + String("\002 :("));
      return;
   }
   
   if (Utils::isWildcard(rest)) {
      from->sendLine("Mass op is not allowed. Sorry :(");
      return;
   }
   
   String target;
   
   if (rest.length() == 0) {
      target = nick;
   } else {
      target = rest;
   }
   
   User *u = from->cnx->bot->channelList->getChannel(channel)->getUser(target);
   if (!u) {
      from->sendLine(String("I couldn't find \002") + target + 
		       "\002 on \002" + channel + "\002");
      return;
   }
   
   if (!(u->mode & User::OP_MODE)) {
      from->cnx->queue->sendChannelMode(channel, "+o", target);
   } else {
      if (target == nick) {
	 from->sendLine(String("But aren't you already a channel op on \002") 
			  + channel + "\002??!!");
      } else {
	 from->sendLine(String("But \002") + target + 
			  "\002 is already a channel op on\002 " + channel +
			  "\002!");
      }
   }
}

