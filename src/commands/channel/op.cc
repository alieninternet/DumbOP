
#include "commands.h"
#include "flags.h"
#include "bot.h"

/* Op
 * Original 14/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::Op(ServerConnection *cnx, Person *from,
		      String channel, String rest)
{
   String nick = from->getNick();
   
   if (!cnx->bot->iAmOp(channel)) {
      from->sendNotice(String("I'm not a channel op on \002") +
		       channel + String("\002 :("));
      return;
   }
   
   if (Utils::isWildcard(rest)) {
      from->sendNotice("Mass op is not allowed. Sorry :(");
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
   
   if (!(u->mode & User::OP_MODE)) {
      cnx->queue->sendChannelMode(channel, "+o", target);
   } else {
      if (target == nick) {
	 from->sendNotice(String("But aren't you already a channel op on \002") 
			  + channel + "\002??!!");
      } else {
	 from->sendNotice(String("But \002") + target + 
			  "\002 is already a channel op on\002 " + channel +
			  "\002!");
      }
   }
}

