
#include "config.h"

#include "parser.h"
#include "bot.h"


/* parseUser001 - RPL_WELCOME
 * Original, Simon Butcher <simonb@alien.net.au>
 */
void Parser::parseUser001(Person *from, String rest)
{
   String temp = "";
   StringTokens st(rest);
   String realNick = st.nextToken();
   
   from->cnx->bot->connected = true;
   
//   from->cnx->queue->sendUserMode(from->cnx->bot->nickName, "+iw");

   /*******************************************************************/
   from->cnx->queue->sendNickopIdent(AUSTNET_PASSWORD);
   /*******************************************************************/
   
//   from->cnx->queue->sendWhois(from->cnx->bot->nickName);
   
   // Force a join on each channel we are requested to join..
   for (map<String, wantedChannel *, less<String> >::iterator it = 
	from->cnx->bot->wantedChannels.begin();
	it != from->cnx->bot->wantedChannels.end(); ++it) {
      from->cnx->queue->sendJoin((*it).first, (*it).second->key);
   }
   
   from->cnx->bot->logLine(String("Connected to server ") +
		     from->cnx->bot->serverList->currentServer()->getHostName() + 
		     " (" +  
		     String((long)from->cnx->bot->serverList->currentServer()->getPort()) +
		     ").");
}

