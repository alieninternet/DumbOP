
#include "config.h"
#include "commands.h"
#include "flags.h"
#include "bot.h"

/* BanList
 * Original 15/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::BanList(ServerConnection *cnx, Person *from,
			   String channel, String rest)
{
   Channel *c = cnx->bot->channelList->getChannel(channel);
   from->sendNotice(String("\002Banlist for channel\002 ") +
		    channel + "\002:\002");
   from->sendNotice("\002Mask                           Expires (seconds)\002");
   for (vector<BanEntry *>::iterator it = c->channelBanlist.begin();
	it != c->channelBanlist.end(); ++it) {
      if ((*it)->getExpirationDate() == -1) {
	 from->sendNotice((*it)->getMask().pad(30) + " -1");
      } else {
	 from->sendNotice((*it)->getMask().pad(30) + " " + 
			  String((long)((*it)->getExpirationDate() - 
					cnx->bot->currentTime.time)));
      }
   }
   from->sendNotice("\002End of banlist.\002");
}

