
#include "commands.h"
#include "flags.h"
#include "bot.h"

/* Join - Join a channel
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::Join(ServerConnection *cnx, Person *from,
			String channel, String rest)
{
   StringTokens st(rest);
   channel = st.nextToken();
   
   if (!Utils::isValidChannelName(channel)) {
      from->sendNotice(String("\002") + channel +
		       " is not a valid channel name\002");
      return;
   }
   
   /* We change the key only if we are not on the channel. We do not trust
    * the user...
    */
   if (!cnx->bot->channelList->getChannel(channel)) {
      if (cnx->bot->wantedChannels[channel]) {
	 cnx->bot->wantedChannels[channel]->key = st.rest();
      } else {
	 cnx->bot->wantedChannels[channel] = new wantedChannel("", "", st.rest(),
							       0, 0, "", "");
      }
   }
   
   cnx->queue->sendJoin(channel, cnx->bot->wantedChannels[channel]->key);
}

