
#include "config.h"

#include "parser.h"
#include "bot.h"
#include "channel.h"


/* parseUser324 - RPL_CHANNELMODEIS
 * Original, Simon Butcher <simonb@alien.net.au>
 */
void Parser::parseUser324(Person *from, String rest)
{
   StringTokens st(rest);
   st.nextToken();
   String channel = st.nextToken();
   
   if (Channel *c = from->cnx->bot->channelList->getChannel(channel)) {
      if (c) {
	 c->parseMode(from, st.rest());
      }
   }
}


