
#include "commands.h"
#include "flags.h"
#include "bot.h"

/* Keep - Channel modes to lock
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::Keep(ServerConnection *cnx, Person *from,
			String channel, String rest)
{
   String nick = from->getNick();
   
   if (rest.length() == 0) {
      from->sendNotice("\002No channel modes specified.\002");
      return;
   }
   
   cnx->bot->channelList->getChannel(channel)->keepModes = rest;
 
   from->sendNotice(String("Keeping modes \002") + rest + 
		    String("\002 on channel \002") + channel +
		    String("\002."));
}

