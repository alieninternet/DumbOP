
#include "config.h"

#include "commands.h"
#include "flags.h"
#include "bot.h"

/* Keep - Channel modes to lock
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::Keep(Person *from, String channel, String rest)
{
   String nick = from->getNick();
   
   if (rest.length() == 0) {
      from->sendLine("\002No channel modes specified.\002");
      return;
   }
   
   from->cnx->bot->channelList->getChannel(channel)->keepModes = rest;
 
   from->sendLine(String("Keeping modes \002") + rest + 
		    String("\002 on channel \002") + channel +
		    String("\002."));
}

