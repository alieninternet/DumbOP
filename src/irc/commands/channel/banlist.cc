
#include "config.h"

#include "commands.h"
#include "flags.h"
#include "bot.h"

/* BanList
 * Original 15/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::BanList(Person *from, String channel, String rest)
{
   Channel *c = from->cnx->bot->channelList->getChannel(channel);
   from->sendLine(String("\002Banlist for channel\002 ") +
		    channel + "\002:\002");
   from->sendLine("\002Mask                           Expires (seconds)\002");
   for (vector<BanEntry *>::iterator it = c->channelBanlist.begin();
	it != c->channelBanlist.end(); ++it) {
      if ((*it)->getExpirationDate() == -1) {
	 from->sendLine((*it)->getMask().pad(30) + " -1");
      } else {
	 from->sendLine((*it)->getMask().pad(30) + " " + 
			  String((long)((*it)->getExpirationDate() - 
					from->cnx->bot->currentTime.time)));
      }
   }
   from->sendLine("\002End of banlist.\002");
}

