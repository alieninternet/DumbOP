/* src/irc/commands/channel/voice.cc
 * VOICE command
 * Copyright (c) 2001 Alien Internet Services
 */

#include "config.h"

#include "commands.h"
#include "flags.h"
#include "bot.h"

/* Voice
 * Original 26/06/01, Pickle <pickle@alien.net.au>
 */
void Commands::Voice(Person *from, String channel, String rest)
{
   String nick = from->getNick();
   
   if (!from->cnx->bot->iAmOp(channel)) {
      from->sendLine(String("I'm not a channel op on \002") +
		       channel + String("\002 :("));
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
   
   if (!(u->mode & User::VOICE_MODE)) {
      from->cnx->queue->sendChannelMode(channel, "+v", target);
   } else {
      if (target == nick) {
	 from->sendLine(String("Hey, you're already voiced on \002") 
			  + channel + "\002!!");
      } else {
	 from->sendLine(String("But \002") + target + 
			  "\002 is already voiced on\002 " + channel +
			  "\002!");
      }
   }
}

