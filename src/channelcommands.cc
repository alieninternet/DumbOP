
#include "commands.h"
#include "usercommands.h"
#include "str.h"
#include "stringtokenizer.h"
#include "flags.h"


/* Ban
 * Original 15/12/00, Pickle <pickle@alien.net.au>
 */
void
UserCommands::Ban(ServerConnection *cnx, Person *from,
                  String channel, String rest)
{
  if (rest.length() == 0) {
    if (from)
      from->sendNotice("\002No nick/mask specified.\002");
    return;
  }

  Message m = Commands::Ban(cnx->bot, channel, rest);
  if (m.getCode() != 0 && from)
    from->sendNotice(m.getMessage());
}

/* BanList
 * Original 15/12/00, Pickle <pickle@alien.net.au>
 */
void
UserCommands::BanList(ServerConnection *cnx, Person *from,
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
void UserCommands::Cycle(ServerConnection *cnx, Person *from,
			 String channel, String rest)
{
   Message m = Commands::Cycle(cnx->bot, channel, rest);
   if (m.getCode() != 0)
     from->sendNotice(m.getMessage());
   else
     from->sendNotice(String("Cycled on channel \002") + channel + 
		      String("\002"));
}

/* Deban
 * Original 15/12/00, Pickle <pickle@alien.net.au>
 */
void
UserCommands::Deban(ServerConnection *cnx, Person *from,
                    String channel, String rest)
{
  if (rest.length() == 0) {
    from->sendNotice("\002No nick/mask specified.\002");
    return;
  }

  Message m = Commands::Deban(cnx->bot, channel, rest);
  if (m.getCode() != 0)
    from->sendNotice(m.getMessage());
}

/* Deop
 * Original 14/12/00, Pickle <pickle@alien.net.au>
 * Needs: Convertion from using commands.c
 */
void UserCommands::Deop(ServerConnection *cnx, Person *from,
			String channel, String rest)
{
   String target = rest;
   
   if (target.length() == 0)
     target = from->getNick();
   
   Message m = Commands::Deop(cnx->bot, channel, target);
   if (m.getCode() != 0)
     from->sendNotice(m.getMessage());
}

/* Invite
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 * Needs: Convert from command.c dependancy
 * Needs: Self-invite ability without specifying originating nick
 */
void
UserCommands::Invite(ServerConnection *cnx, Person *from,
                     String channel, String rest)
{
  Message m = Commands::Invite(cnx->bot, channel, rest);
  if (m.getCode() < 0)
    from->sendNotice(m.getMessage());
  from->sendNotice(String("\002Inviting\002 ") + rest + " \002on channel\002 " + channel);
}

/* Join - Join a channel
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 */
void
UserCommands::Join(ServerConnection *cnx, Person *from,
                   String channel, String rest)
{
  StringTokenizer st(rest);
  channel = st.nextToken();

  if (!Utils::isValidChannelName(channel)) {
    from->sendNotice(String("\002") + channel +
                     " is not a valid channel name\002");
    return;
  }

  // We change the key only if we are not on the channel. We do not trust
  // the user...
  if (!cnx->bot->channelList->getChannel(channel)) {
    if (cnx->bot->wantedChannels[channel])
      cnx->bot->wantedChannels[channel]->key = st.rest();
    else {
      cnx->bot->wantedChannels[channel] = new wantedChannel("", "", st.rest(),
							    0, 0, "", "");
    }
  }
  cnx->queue->sendJoin(channel, cnx->bot->wantedChannels[channel]->key);
}

/* Keep - Channel modes to lock
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 */
void
UserCommands::Keep(ServerConnection *cnx, Person *from,
                   String channel, String rest)
{
  String nick = from->getNick();

  if (rest.length() == 0) {
    from->sendNotice("\002No channel modes specified.\002");
    return;
  }

  cnx->bot->channelList->getChannel(channel)->keepModes = rest;
  from->sendNotice(String("\002Keeping modes\002 ") +
                    rest + " \002on channel\002 " +
                    channel);
}

/* Kick - Kick a user
 * Original 17/12/00, Pickle <pickle@alien.net.au>
 */
void UserCommands::Kick(ServerConnection *cnx, Person *from,
			String channel, String rest)
{
   String nick = from->getNick();
   
   Channel * c = cnx->bot->channelList->getChannel(channel);
   
   if (rest.length() == 0) {
      from->sendNotice("Whom dost thou kicketh?");
      return;
   }
   
   StringTokenizer st(rest);
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
      if (u->userListItem->flags & USERFLAG_PROTECT_NOKICK)
	from->sendNotice(String("I don't want to kick \002") +
			 who + "\002 on \002" + channel +
			 "\002 (Protected)");
      else {
	 if (st.rest())
	   cnx->queue->sendKick(channel, who, String("[") + from->getNick() +
				String("] ") + st.rest());
      }
      
   }
}

/* Merge with ban. */
void
UserCommands::KickBan(ServerConnection *cnx, Person *from,
                      String channel, String rest)
{
  StringTokenizer st(rest);

  Ban(cnx, 0, channel, st.nextToken());
  Kick(cnx, from, channel, rest);
}

/* Mode - Change a channel mode
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 */
void
  UserCommands::Mode(ServerConnection *cnx, Person *from,
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
void UserCommands::Op(ServerConnection *cnx, Person *from,
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
   
   if (rest.length() == 0)
     target = nick;
   else
     target = rest;
   
   User *u = cnx->bot->channelList->getChannel(channel)->getUser(target);
   if (!u) {
      from->sendNotice(String("I couldn't find \002") + target + 
		       "\002 on \002" + channel + "\002");
      return;
   }
   
   if (!(u->mode & User::OP_MODE))
     cnx->queue->sendChannelMode(channel, "+o", target);
   else {
      if (target == nick)
	from->sendNotice(String("But aren't you already a channel op on \002") 
			 + channel + "\002??!!");
      else
	from->sendNotice(String("But \002") + target + 
			 "\002 is already a channel op on\002 " + channel +
			 "\002!");
   }
}

/* Part - leave a channel
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 */
void UserCommands::Part(ServerConnection *cnx, Person *from,
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
void UserCommands::Rules(ServerConnection *cnx, Person *from,
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
void
UserCommands::Topic(ServerConnection *cnx, Person *from,
                    String channel, String rest)
{
  String nick = from->getNick();

  if (rest.length() == 0) {
    if (cnx->bot->channelList->getChannel(channel)->channelTopic == "")
      from->sendNotice(String("No topic is set for channel\002 ") +
                        channel + "\002.");
    else
      from->sendNotice(String("\037Topic for\002 ") +
                        channel + " \002is:\037 " +
                        cnx->bot->channelList->getChannel(channel)->channelTopic);
  } else {
    if (cnx->bot->channelList->getChannel(channel)->lockedTopic)
      from->sendNotice(String("Topic is locked on channel ") +
                       channel);
    else
      cnx->queue->sendTopic(channel, rest);
  }
}

/* Voice
 * Original 26/06/01, Pickle <pickle@alien.net.au>
 */
void UserCommands::Voice(ServerConnection *cnx, Person *from,
			 String channel, String rest)
{
   String nick = from->getNick();
   
   if (!cnx->bot->iAmOp(channel)) {
      from->sendNotice(String("I'm not a channel op on \002") +
		       channel + String("\002 :("));
      return;
   }
   
   String target;
   
   if (rest.length() == 0)
     target = nick;
   else
     target = rest;
   
   User *u = cnx->bot->channelList->getChannel(channel)->getUser(target);
   if (!u) {
      from->sendNotice(String("I couldn't find \002") + target + 
		       "\002 on \002" + channel + "\002");
      return;
   }
   
   if (!(u->mode & User::VOICE_MODE))
     cnx->queue->sendChannelMode(channel, "+v", target);
   else {
      if (target == nick)
	from->sendNotice(String("Hey, you're already voiced on \002") 
			 + channel + "\002!!");
      else
	from->sendNotice(String("But \002") + target + 
			 "\002 is already voiced on\002 " + channel +
			 "\002!");
   }
}

