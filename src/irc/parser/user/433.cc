
#include "config.h"

#include "parser.h"
#include "bot.h"


/* parseUser433 - ERR_NICKNAMEINUSE
 * Original, Simon Butcher <simonb@alien.net.au>
 */
void Parser::parseUser433(Person *from, String rest)
{
   if (from->cnx->bot->connected) {
      return;
   }

   if (from->cnx->bot->nickName.length() == 9) {
      int i;
      
      for (i = 0; (i < from->cnx->bot->nickName.length() && 
		   from->cnx->bot->nickName[i] == '_'); 
	   i++);
      
      if (i < from->cnx->bot->nickName.length()) {
	 from->cnx->bot->nickName = 
	   from->cnx->bot->nickName.subString(0, i-1) + "_" + 
	   from->cnx->bot->nickName.subString(i+1);
      } else {
	 from->cnx->bot->nickName = from->cnx->bot->nickName.subString(0, 4) +
	   String((long)(rand() % 10000));
      }
   } else {
      from->cnx->bot->nickName = from->cnx->bot->nickName + "_";
   }
   
   from->cnx->queue->sendNick(from->cnx->bot->nickName);
}


