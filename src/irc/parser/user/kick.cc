
#include "config.h"

#include "parser.h"
#include "bot.h"
#include "channel.h"
#include "flags.h"


void Parser::parseUserKick(Person *from, String rest)
{
   StringTokens st(rest);
   String channel = st.nextToken();
   String target = st.nextToken();
   String reason = st.rest().subString(1);
   
   if (target == from->cnx->bot->nickName) {
      from->cnx->bot->logLine(from->getAddress() + " kicked me out of channel " +
			channel + " (" + reason + ").");
      from->cnx->queue->sendJoin(channel, from->cnx->bot->channelList->getChannel(channel)->channelKey);
      from->cnx->bot->channelList->delChannel(channel);
   } else {
      // Get the channel info
      Channel *c = from->cnx->bot->channelList->getChannel(channel);
      
      // Make sure that worked..
      if (!c) {
	 return;
      }
      
      // Delete the nickname from that channel
      c->delNick(target);

      // If it is a game channel, the game engine may need this data too.
      if (from->cnx->bot->wantedChannels[c->channelName]->flags & 
	  CHANFLAG_ALLOW_GAMES) {
	 from->cnx->bot->games->parseLeave(c, from);
      }
   }
}


