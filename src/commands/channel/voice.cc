
#include "commands.h"
#include "flags.h"
#include "bot.h"

/* Voice
 * Original 26/06/01, Pickle <pickle@alien.net.au>
 */
void Commands::Voice(ServerConnection *cnx, Person *from,
			 String channel, String rest)
{
   String nick = from->getNick();
   
   if (!cnx->bot->iAmOp(channel)) {
      from->sendNotice(String("I'm not a channel op on \002") +
		       channel + String("\002 :("));
      return;
   }
   
   String target;
   
   if (rest.length() == 0) {
      target = nick;
   } else {
      target = rest;
   }
      
   User *u = cnx->bot->channelList->getChannel(channel)->getUser(target);
   if (!u) {
      from->sendNotice(String("I couldn't find \002") + target + 
		       "\002 on \002" + channel + "\002");
      return;
   }
   
   if (!(u->mode & User::VOICE_MODE)) {
      cnx->queue->sendChannelMode(channel, "+v", target);
   } else {
      if (target == nick) {
	 from->sendNotice(String("Hey, you're already voiced on \002") 
			  + channel + "\002!!");
      } else {
	 from->sendNotice(String("But \002") + target + 
			  "\002 is already voiced on\002 " + channel +
			  "\002!");
      }
   }
}

