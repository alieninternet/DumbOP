
#include "config.h"

#include "parser.h"
#include "bot.h"
#include "channel.h"


/* parseUser366 - RPL_ENDOFNAMES
 * Original, Simon Butcher <simonb@alien.net.au>
 */
void Parser::parseUser366(Person *from, String rest)
{
   StringTokens st(rest);
   st.nextToken();
   String ch = st.nextToken();

   if (Channel *c = from->cnx->bot->channelList->getChannel(ch)) {
      c->joined = true;
   }
}


