
#include "config.h"

#include "parser.h"
#include "bot.h"
#include "channel.h"


/* parseUser315 - RPL_ENDOFWHO
 * Original, Simon Butcher <simonb@alien.net.au>
 */
void Parser::parseUser315(Person *from, String rest)
{
   StringTokens st(rest);
   st.nextToken();
   String channel = st.nextToken();
   Channel *c = from->cnx->bot->channelList->getChannel(channel);
   
   if (!c) {
      return;
   }
   
   c->gotWho = true;
}


