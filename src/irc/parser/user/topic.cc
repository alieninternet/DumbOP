
#include "config.h"

#include "parser.h"
#include "bot.h"
#include "channel.h"


void Parser::parseUserTopic(Person *from, String rest)
{
   StringTokens st(rest);
   String channel = st.nextToken();
   String newTopic = st.rest().subString(1);
   Channel *c = from->cnx->bot->channelList->getChannel(channel);
   
  if (!c) {
     return;
  }

   if (c->lockedTopic && from->getNick() != from->cnx->bot->nickName) {
      from->cnx->queue->sendTopic(channel, c->channelTopic);
   }
   
   c->channelTopic = newTopic;
}


