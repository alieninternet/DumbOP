
#include "config.h"

#include "parser.h"
#include "bot.h"


/* parseUser473 - RPL_UNAVAILRESOURCE
 * Original, Simon Butcher <simonb@alien.net.au>
 */
void Parser::parseUser473(Person *from, String rest)
{
   StringTokens st(rest);
   st.nextToken();
   
   from->cnx->bot->logLine(String("Unable to join channel ") + st.nextToken() + ".");
}

