
#include "config.h"

#include "parser.h"


void Parser::parseUserPing(Person *from, String rest)
{
   from->cnx->queue->sendPong(rest);
}


