
#include "config.h"
#include "commands.h"
#include "flags.h"
#include "bot.h"

/* Invite
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 * Needs: Convert from command.c dependancy
 * Needs: Self-invite ability without specifying originating nick
 */
void Commands::Invite(ServerConnection *cnx, Person *from,
			  String channel, String rest)
{
//   Message m = Commands::Invite(cnx->bot, channel, rest);
//   if (m.getCode() < 0)
//     from->sendNotice(m.getMessage());
//   from->sendNotice(String("\002Inviting\002 ") + rest + " \002on channel\002 " + channel);
//  CHECK_CONNECTION;
//
//  if (!bot->iAmOp(channel))
//    return NotChannelOp(channel);
//
//  QUEUE->sendInvite(channel, who);
//
//  return Ok;
}

