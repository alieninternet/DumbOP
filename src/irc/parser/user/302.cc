
#include "config.h"

#include "parser.h"
#include "bot.h"


/* parseUser302 - RPL_USERHOST
 * Original, Simon Butcher <simonb@alien.net.au>
 */
void Parser::parseUser302(Person *from, String rest)
{
   unsigned long num = from->cnx->bot->receivedUserhostID++;
   StringTokens st(rest);
   
   st.nextToken(':');
   
   if (st.rest().length()) {
      st.nextToken('=');
      String parameters = st.rest();
      parameters = parameters.subString(1);
      from->cnx->bot->userhostMap[num] = parameters;
   } else {
      from->cnx->bot->userhostMap[num] = "";
   }
}


