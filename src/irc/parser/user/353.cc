
#include "config.h"

#include "parser.h"
#include "bot.h"
#include "channel.h"
#include "user.h"


/* parseUser353 - RPL_NAMESREPLY
 * Original, Simon Butcher <simonb@alien.net.au>
 */
void Parser::parseUser353(Person *from, String rest)
{
   int mode = 0;
   String nick;
   
   StringTokens st(rest);
   st.nextToken(); st.nextToken();
   
   Channel * c = from->cnx->bot->channelList->getChannel(st.nextToken());
   
   if (!c) {
      return;
   }
   
   StringTokens st2(st.nextToken(':'));
   
   while (st2.hasMoreTokens()) {
      nick = st2.nextToken();

      if (nick[0] == '@') {
	 mode = User::OP_MODE;
	 nick = nick.subString(1);
      } else if (nick[0] == '+') {
	 mode = User::VOICE_MODE;
	 nick = nick.subString(1);
      }
      
      c->addNick(nick, "", mode, 0, true);
   }
}


