
#include "config.h"

#include "parser.h"
#include "bot.h"
#include "channel.h"
#include "flags.h"


void Parser::parseUserPart(Person *from, String rest)
{
   String n = from->getNick();
   StringTokens st(rest);
   String channel = st.nextToken();
   
   if (n.toLower() == from->cnx->bot->nickName.toLower()) {
      from->cnx->bot->logLine(String("Left channel ") + channel + ".");
      from->cnx->bot->channelList->delChannel(channel);
   } else {
      Channel * c = from->cnx->bot->channelList->getChannel(channel);
      
      if (!c) {
	 return;
      }
      
      c->delNick(n);
      
      if (c->countOp == 0 && c->count == 1) {
	 from->cnx->queue->sendPart(channel, "");
	 from->cnx->queue->sendJoin(channel, from->cnx->bot->wantedChannels[channel]->key);
      }

      // If it is a game channel, the game engine may need this data too.
      if (from->cnx->bot->wantedChannels[c->channelName]->flags & 
	  CHANFLAG_ALLOW_GAMES) {
	 from->cnx->bot->games->parseLeave(c, from);
      }
   }
}


