
#include "config.h"

#include "parser.h"
#include "bot.h"


void Parser::parseUserPong(Person *from, String rest)
{
   from->cnx->lag = (((from->cnx->bot->currentTime.time * 1000) + 
		from->cnx->bot->currentTime.millitm) -
	       ((from->cnx->pingTime.time * 1000) +
		from->cnx->pingTime.millitm));
   from->cnx->bot->sentPing = false;
}


