
#include "config.h"

#include "commands.h"
#include "flags.h"
#include "bot.h"

/* Ban
 * Original 15/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::Ban(Person *from, String channel, String rest)
{
   if (rest.length() == 0) {
      if (from)
	from->sendLine("\002No nick/mask specified.\002");
      return;
   }
   
//  Message m = Commands::Ban(from->cnx->bot, channel, rest);
//  if (m.getCode() != 0 && from)
//    from->sendLine(m.getMessage());
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
//    return NotFound(who);
//
//  dest = Utils::makeWildcard(dest);
//  Mask m(dest);
//
//  for (list<UserListItem *>::iterator it = bot->userList->l.begin();
//       it != bot->userList->l.end();
//       it++)
//    if (m.matches((*it)->mask) &&
//        (*it)->channelMask.matches(channel) &&
//        (*it)->prot >= User::NO_BAN)
//      return UserProtected(who, channel);
//
//  for (vector<BanEntry *>::iterator it = c->channelBanlist.begin();
//       it != c->channelBanlist.end(); ++it)
//    if (m.matches((*it)->banMask) && (*it)->banMask.getMask() != m.getMask())
//      QUEUE->sendChannelMode(channel, "-b", (*it)->banMask.getMask());
//  
//  QUEUE->sendChannelMode(channel, "+b", dest);
//
//  return Ok;
}


