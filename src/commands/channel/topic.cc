
#include "config.h"
#include "commands.h"
#include "flags.h"
#include "bot.h"

/* Topic - Change the channel's topic
 * Original 15/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::Topic(ServerConnection *cnx, Person *from,
			 String channel, String rest)
{
   String nick = from->getNick();
   
   if (rest.length() == 0) {
      if (cnx->bot->channelList->getChannel(channel)->channelTopic == "") {
	 from->sendNotice(String("No topic is set for channel\002 ") +
			  channel + "\002.");
      } else {
	 from->sendNotice(String("\037Topic for\002 ") +
			  channel + " \002is:\037 " +
			  cnx->bot->channelList->getChannel(channel)->channelTopic);
      }
   } else {
      if (cnx->bot->channelList->getChannel(channel)->lockedTopic) {
	 from->sendNotice(String("Topic is locked on channel ") +
			  channel);
      } else {
	 cnx->queue->sendTopic(channel, rest);
      }
   }
}

