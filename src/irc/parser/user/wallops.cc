
#include "config.h"

#include "parser.h"
#include "bot.h"


void Parser::parseUserWallops(Person *from, String rest)
{
   from->cnx->bot->logLine(String("WALLOPS from ") + from->getAddress() +
			   String(" (") + rest + String(")"));
}
