
#include "config.h"
#include "commands.h"
#include "utils.h"

/* Nick - Change nickname
 * Original 14/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::Nick(ServerConnection *cnx, Person *from,
			String channel, String rest)
{
   StringTokens st(rest);
   String nick = st.nextToken();
   
   if (rest == "") {
      from->sendNotice(String("\002No nickname given.\002"));
      return;
   }
   
   if (!Utils::isValidNickName(nick)) {
      from->sendNotice(String("\002") + nick +
		       " is not a valid nickname\002");
      return;
   }
   
   cnx->bot->wantedNickName = nick;
   cnx->queue->sendNick(nick);
}

