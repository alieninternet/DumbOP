
#include "config.h"

#include "parser.h"
#include "bot.h"
#include "channel.h"


/* parseUser367 - RPL_BANLIST
 * Original, Simon Butcher <simonb@alien.net.au>
 */
void Parser::parseUser367(Person *from, String rest)
{
   StringTokens st(rest);
   st.nextToken();
   String ch = st.nextToken();

   if (Channel *c = from->cnx->bot->channelList->getChannel(ch)) {
      c->addBan(st.nextToken(), -1);
   }
}


