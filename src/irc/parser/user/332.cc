
#include "config.h"

#include "parser.h"
#include "bot.h"
#include "channel.h"


/* parseUser332 - RPL_TOPIC
 * Original, Simon Butcher <simonb@alien.net.au>
 */
void Parser::parseUser332(Person *from, String rest)
{
   StringTokens st(rest);
   st.nextToken();
   String channel = st.nextToken();
   
   if (Channel *c = from->cnx->bot->channelList->getChannel(channel)) {
    if (c) {
       c->channelTopic = st.rest().subString(1);
    }
   }
}


