
#include "config.h"

#include "parser.h"
#include "bot.h"


void Parser::parseUserError(Person *from, String rest)
{
   from->cnx->bot->logLine(String("Error from server ") +
			   from->cnx->bot->serverList->currentServer()->getHostName() +
			   " (" + 
			   String((long)from->cnx->bot->serverList->currentServer()->getPort()) +
			   ").");
   from->cnx->bot->nextServer();
}


