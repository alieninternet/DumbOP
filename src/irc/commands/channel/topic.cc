/* src/irc/commands/channel/topic.cc
 * TOPIC command
 * Copyright (c) 2000 Alien Internet Services
 */

#include "config.h"

#include "commands.h"
#include "flags.h"
#include "bot.h"

/* Topic - Change the channel's topic
 * Original 15/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::Topic(Person *from, String channel, String rest)
{
   String nick = from->getNick();
   
   if (rest.length() == 0) {
      if (from->cnx->bot->channelList->getChannel(channel)->channelTopic == "") {
	 from->sendLine(String("No topic is set for channel\002 ") +
			  channel + "\002.");
      } else {
	 from->sendLine(String("\037Topic for\002 ") +
			  channel + " \002is:\037 " +
			  from->cnx->bot->channelList->getChannel(channel)->channelTopic);
      }
   } else {
      if (from->cnx->bot->channelList->getChannel(channel)->lockedTopic) {
	 from->sendLine(String("Topic is locked on channel ") +
			  channel);
      } else {
	 from->cnx->queue->sendTopic(channel, rest);
      }
   }
}

