
#include "config.h"

#include "parser.h"
#include "bot.h"
#include "channel.h"
#include "flags.h"


void Parser::parseUserQuit(Person *from, String rest)
{
   String n = from->getNick();
   
   if (n == from->cnx->bot->nickName) {
      from->cnx->bot->stop = true;
   }
   
   for (map<String, Channel *, less<String> >::iterator it =
	from->cnx->bot->channelList->begin();
	it != from->cnx->bot->channelList->end();
	++it) {
      (*it).second->delNick(n);

      // If it is a game channel, the game engine may need this data too.
      if (from->cnx->bot->wantedChannels[(*it).second->channelName]->flags & 
	  CHANFLAG_ALLOW_GAMES) {
	 from->cnx->bot->games->parseLeave((*it).second, from);
      }
   }
}


