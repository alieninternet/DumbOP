
#include "config.h"

#include "parser.h"
#include "bot.h"
#include "channel.h"
#include "utils.h"


void Parser::parseUserMode(Person *from, String rest)
{
   StringTokens st(rest);
   String ch = st.nextToken();
   String modes = st.rest();
   
   if (Utils::isChannel(ch)) {
      Channel *c = from->cnx->bot->channelList->getChannel(ch);
      
      if (!c) {
	 return;
      }
      
      if (from) {
	 c->parseMode(from, modes);
      } else {
	 c->parseMode(0, modes);
      }
   }
}


