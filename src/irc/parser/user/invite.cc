
#include "config.h"

#include "parser.h"
#include "bot.h"


void Parser::parseUserInvite(Person *from, String rest)
{
   String nick = from->getNick();
   StringTokens st(rest);
   st.nextToken(':');
   String channel = st.rest();
   
   if (from->cnx->bot->wantedChannels.find(channel) !=
       from->cnx->bot->wantedChannels.end()) {
      from->cnx->queue->sendJoin(channel, from->cnx->bot->wantedChannels[channel]->key);
   }
}


