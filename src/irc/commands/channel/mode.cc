
#include "config.h"

#include "commands.h"
#include "flags.h"
#include "bot.h"

/* Mode - Change a channel mode
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::Mode(Person *from, String channel, String rest)
{
   String nick = from->getNick();
   
   if (rest.length() == 0) {
      from->sendLine("\002I need a parameter for this command.\002");
      return;
   }
   
   if (!from->cnx->bot->iAmOp(channel)) {
      from->sendLine(String("\002I am not channel op on\002 ") +
		       channel);
      return;
   }
   
   from->cnx->queue->sendChannelMode(String("MODE ") + channel + " " + rest);
}

