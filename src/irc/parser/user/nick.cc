
#include "config.h"

#include "parser.h"
#include "bot.h"
#include "channel.h"


void Parser::parseUserNick(Person *from, String rest)
{
   String oldNick = from->getNick();
   String newNick = rest.subString(1);
   
   for (map<String, Channel *, less<String> >::iterator it = 
	from->cnx->bot->channelList->begin(); 
	it != from->cnx->bot->channelList->end(); ++it) {
      if ((*it).second->hasNick(oldNick)) {
	 (*it).second->changeNick(oldNick, newNick);
      }
   }
}


