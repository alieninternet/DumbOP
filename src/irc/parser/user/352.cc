
#include "config.h"

#include "parser.h"
#include "bot.h"
#include "user.h"


/* parseUser352 - RPL_WHOREPLY
 * Original, Simon Butcher <simonb@alien.net.au>
 */
void Parser::parseUser352(Person *from, String rest)
{
   StringTokens st(rest);
   st.nextToken();
   String ch = st.nextToken();
   String uh = st.nextToken() + "@";
   uh = uh + st.nextToken();
   st.nextToken();
   String n = st.nextToken();
   String m = st.nextToken();
   int mode = 0;
   
   for (int i = 0; i < m.length(); i++)
     switch (m[i]) {
      case 'H': 
	break;
      case 'G': 
	mode |= User::AWAY_MODE; 
	break;
      case '*': 
	mode |= User::IRCOP_MODE; 
	break;
      case '@': 
	mode |= User::OP_MODE; 
	break;
      case '+': 
	mode |= User::VOICE_MODE; 
	break;
    }
   
   if (Channel *c = from->cnx->bot->channelList->getChannel(ch)) {
      if (c) {
	 c->addNick(n, uh, mode, from->cnx->bot->userList);
      }
   }
}


