
#include "dccparser.h"
#include "dccperson.h"
#include "macros.h"
#include "parser.h"
#include "bot.h"

void
DCCParser::parseLine(DCCConnection *cnx, String line)
{
  Person * from = new DCCPerson(cnx);
  Parser::parseMessage(cnx->bot->serverConnection, from, cnx->bot->nickName,
                       line);
  delete from;
}
