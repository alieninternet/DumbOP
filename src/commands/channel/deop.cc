
#include "commands.h"
#include "flags.h"
#include "bot.h"

/* Deop
 * Original 14/12/00, Pickle <pickle@alien.net.au>
 * Needs: Convertion from using commands.c
 */
void Commands::Deop(ServerConnection *cnx, Person *from,
			String channel, String rest)
{
   String target = rest;
   
   if (target.length() == 0) {
      target = from->getNick();
   }
   
//   Message m = Commands::Deop(cnx->bot, channel, target);
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
//  if (!Utils::isWildcard(who)) {
//    User *u = c->getUser(who);
//    if (!u)
//      return UserNotFound(who, channel);
//    if (!(u->mode & User::OP_MODE))
//      return UserNotOp(who, channel);
//    if (u->getProt() >= User::NO_DEOP)
//      return UserProtected(who, channel);
//    QUEUE->sendChannelMode(channel, "-o", who);
//  } else {
//    Mask m(who);
//    for (map<String, User *, less<String> >::iterator
//           it = c->channelMemory.begin();
//         it != c->channelMemory.end(); ++it) {
//      if (m.matches((*it).second->nick + "!" +
//                    (*it).second->userhost) &&
//          (*it).second->getProt() < User::NO_DEOP &&
//          ((*it).second->mode & User::OP_MODE))
//        QUEUE->sendChannelMode(channel, "-o", (*it).second->nick);
//    }
//  }
//
//  return Ok;
}

