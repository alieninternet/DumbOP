
#include "config.h"

#include "parser.h"


/* parseUser401 - ERR_NOSUCHNICK
 * Original, Simon Butcher <simonb@alien.net.au>
 */
void Parser::parseUser401(Person *from, String rest)
{
   StringTokens st(rest);
   st.nextToken();
   String nick = st.nextToken();
}


