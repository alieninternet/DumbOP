
#include "config.h"

#include "commands.h"
#include "flags.h"
#include "bot.h"
#include "utils.h"

/* Join - Join a channel
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::Join(Person *from, String channel, String rest)
{
   StringTokens st(rest);
   channel = st.nextToken();
   
   if (!Utils::isValidChannelName(channel)) {
      from->sendLine(String("\002") + channel +
		       " is not a valid channel name\002");
      return;
   }
   
   /* We change the key only if we are not on the channel. We do not trust
    * the user...
    */
   if (!from->cnx->bot->channelList->getChannel(channel)) {
      if (from->cnx->bot->wantedChannels[channel]) {
	 from->cnx->bot->wantedChannels[channel]->key = st.rest();
      } else {
	 from->cnx->bot->wantedChannels[channel] = 
	   new wantedChannel("", "", st.rest(), 0, 0, "", "");
      }
   }
   
   from->cnx->queue->sendJoin(channel, 
			      from->cnx->bot->wantedChannels[channel]->key);
}

