
#include "config.h"
#include "commands.h"
#include "flags.h"
#include "bot.h"

/* Deban
 * Original 15/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::Deban(ServerConnection *cnx, Person *from,
			 String channel, String rest)
{
   if (rest.length() == 0) {
      from->sendNotice("\002No nick/mask specified.\002");
      return;
   }
   
//   Message m = Commands::Deban(cnx->bot, channel, rest);
//   if (m.getCode() != 0)
//     from->sendNotice(m.getMessage());
//  CHECK_CONNECTION;
//
//  Channel *c = CHANNEL(channel);
//
//  if (!c)
//    return NotOnChannel(channel);
//
//  if (!bot->iAmOp(channel))
//    return NotChannelOp(channel);
//
//  String dest;
//
//  if (!Utils::isWildcard(who))
//    dest = bot->getUserhost(channel, who);
//  else
//    dest = who;
//
//  if (dest.length() == 0)
//    return UserNotFound(who, channel);
//
//  dest = Utils::makeWildcard(dest);
//  Mask m(dest);
//  
//  for (vector<BanEntry *>::iterator it = c->channelBanlist.begin();
//       it != c->channelBanlist.end(); ++it)
//    if (m.matches((*it)->getMask())) {
//       QUEUE->sendChannelMode(channel, "-b", (*it)->getMask());
//    }
//
//  return Ok;
}

