
#include "commands.h"
#include "flags.h"
#include "bot.h"


/* Ban
 * Original 15/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::Ban(ServerConnection *cnx, Person *from,
		       String channel, String rest)
{
   if (rest.length() == 0) {
      if (from)
	from->sendNotice("\002No nick/mask specified.\002");
      return;
   }
   
//  Message m = Commands::Ban(cnx->bot, channel, rest);
//  if (m.getCode() != 0 && from)
//    from->sendNotice(m.getMessage());
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


/* Cycle
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::Cycle(ServerConnection *cnx, Person *from,
			 String channel, String rest)
{
   // Leave..
   if (rest.length()) {
      cnx->queue->sendPart(channel, String("[Cycle] ") + rest);
   } else {
      cnx->queue->sendPart(channel, String("Cycle"));
   }
   
   // Join..
   cnx->queue->sendJoin(channel, cnx->bot->wantedChannels[channel]->key);
   
   // Explain..
   from->sendNotice(String("Cycled on channel \002") + channel + 
		    String("\002"));
}


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


/* Keep - Channel modes to lock
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::Keep(ServerConnection *cnx, Person *from,
			String channel, String rest)
{
   String nick = from->getNick();
   
   if (rest.length() == 0) {
      from->sendNotice("\002No channel modes specified.\002");
      return;
   }
   
   cnx->bot->channelList->getChannel(channel)->keepModes = rest;
 
   from->sendNotice(String("Keeping modes \002") + rest + 
		    String("\002 on channel \002") + channel +
		    String("\002."));
}


/* Kick - Kick a user
 * Original 17/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::Kick(ServerConnection *cnx, Person *from,
			String channel, String rest)
{
   String nick = from->getNick();
   
   Channel * c = cnx->bot->channelList->getChannel(channel);
   
   if (rest.length() == 0) {
      from->sendNotice("Whom dost thou kicketh?");
      return;
   }
   
   StringTokens st(rest);
   String who = st.nextToken();
   
   if (who.toLower() == from->getNick().toLower()) {
      from->sendNotice("You might sprain something if you kick yourself.");
      return;
   }
   
   if (Utils::isWildcard(who)) {
      User * u = c->getUser(nick);
      if (!u)
	return;
      if (u->getLevel() < User::TRUSTED_USER) {
	 from->sendNotice("You need an higher level to use wildcards.");
	 return;
      }
   }
   
   if (!cnx->bot->iAmOp(channel)) {
      from->sendNotice(String("I am not a channel op on \002") +
		       channel + String("\002 :("));
      return;
   }
   
   if (Utils::isWildcard(who)) {
      Mask m(who);
      for (map<String, User *, less<String> >::iterator it =
           c->channelMemory.begin();
	   it != c->channelMemory.end();
	   ++it)
	if (m.matches((*it).second->nick + "!" +
		      (*it).second->userhost) &&
	    (*it).second->getProt() < User::NO_KICK)
	  cnx->queue->sendKick(channel, (*it).second->nick, st.rest());
   } else {
      User * u = c->getUser(who);
      if (!u) {
	 from->sendNotice(String("Where is \002") +
			  who + "\002 on \002" + channel + "?\002");
	 return;
      }
	 if (st.rest().length())
	   cnx->queue->sendKick(channel, who, String("[") + from->getNick() +
				String("] ") + st.rest());
      
   }
}


/* Mode - Change a channel mode
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::Mode(ServerConnection *cnx, Person *from,
			String channel, String rest)
{
   String nick = from->getNick();
   
   if (rest.length() == 0) {
      from->sendNotice("\002I need a parameter for this command.\002");
      return;
   }
   
   if (!cnx->bot->iAmOp(channel)) {
      from->sendNotice(String("\002I am not channel op on\002 ") +
		       channel);
      return;
   }
   
   cnx->queue->sendChannelMode(String("MODE ") + channel + " " + rest);
}


/* Op
 * Original 14/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::Op(ServerConnection *cnx, Person *from,
		      String channel, String rest)
{
   String nick = from->getNick();
   
   if (!cnx->bot->iAmOp(channel)) {
      from->sendNotice(String("I'm not a channel op on \002") +
		       channel + String("\002 :("));
      return;
   }
   
   if (Utils::isWildcard(rest)) {
      from->sendNotice("Mass op is not allowed. Sorry :(");
      return;
   }
   
   String target;
   
   if (rest.length() == 0) {
      target = nick;
   } else {
      target = rest;
   }
   
   User *u = cnx->bot->channelList->getChannel(channel)->getUser(target);
   if (!u) {
      from->sendNotice(String("I couldn't find \002") + target + 
		       "\002 on \002" + channel + "\002");
      return;
   }
   
   if (!(u->mode & User::OP_MODE)) {
      cnx->queue->sendChannelMode(channel, "+o", target);
   } else {
      if (target == nick) {
	 from->sendNotice(String("But aren't you already a channel op on \002") 
			  + channel + "\002??!!");
      } else {
	 from->sendNotice(String("But \002") + target + 
			  "\002 is already a channel op on\002 " + channel +
			  "\002!");
      }
   }
}


/* Part - leave a channel
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::Part(ServerConnection *cnx, Person *from,
			String channel, String rest)
{
   wantedChannel *w = cnx->bot->wantedChannels[channel];
   cnx->bot->wantedChannels.erase(channel);
   delete w;
   
   cnx->queue->sendPart(channel, rest);
}


/* Rules - Display channel rules
 * Original 18/07/01, Simon Butcher <simonb@alien.net.au>
 */
void Commands::Rules(ServerConnection *cnx, Person *from,
		       String channel, String rest)
{
   // Grab the channel info
   Channel *c = cnx->bot->channelList->getChannel(channel);
   
   // Check if there are rules we need to list..
   if (!cnx->bot->wantedChannels[c->channelName]->rules.length()) {
      // No rules - no comment
      return;
   }
   
   // Otherwise, return the rules.
   from->sendNotice(String("\002") + c->channelName + String(" rules:\002 ") +
		    cnx->bot->wantedChannels[c->channelName]->rules);
}

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


/* Voice
 * Original 26/06/01, Pickle <pickle@alien.net.au>
 */
void Commands::Voice(ServerConnection *cnx, Person *from,
			 String channel, String rest)
{
   String nick = from->getNick();
   
   if (!cnx->bot->iAmOp(channel)) {
      from->sendNotice(String("I'm not a channel op on \002") +
		       channel + String("\002 :("));
      return;
   }
   
   String target;
   
   if (rest.length() == 0) {
      target = nick;
   } else {
      target = rest;
   }
      
   User *u = cnx->bot->channelList->getChannel(channel)->getUser(target);
   if (!u) {
      from->sendNotice(String("I couldn't find \002") + target + 
		       "\002 on \002" + channel + "\002");
      return;
   }
   
   if (!(u->mode & User::VOICE_MODE)) {
      cnx->queue->sendChannelMode(channel, "+v", target);
   } else {
      if (target == nick) {
	 from->sendNotice(String("Hey, you're already voiced on \002") 
			  + channel + "\002!!");
      } else {
	 from->sendNotice(String("But \002") + target + 
			  "\002 is already voiced on\002 " + channel +
			  "\002!");
      }
   }
}

