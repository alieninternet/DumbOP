
#include "macros.h"
#include "message.h"
#include "commands.h"
#include "utils.h"

#define Ok (Message(0, ""))
#define NotOnChannel(c) (Message(-1, String("I am not on channel ") + (c)))
#define NotChannelOp(c) (Message(-2, String("I am not channel op on ") + (c)))
#define UserNotFound(w, c) (Message(-3, (w) + " is not on channel " + (c)))
#define UserNotOp(w, c) (Message(-4, (w) + " is not channel op on " + (c)))
#define UserProtected(w, c) (Message(-5, (w) + " is protected on " + (c)))
#define InvalidNick(n) (Message(-6, (n) + " is not a valid nickname"))
#define InvalidChannel(c) (Message(-7, (c) + " is not a valid channel name"))
#define MassOpNotAllowed (Message(-8, "Mass op is not allowed."))
#define CanNotChangeTopic(c) (Message(-10, String("I can not change topic on ") + (c)))
#define TopicLocked(c) (Message(-11, String("Topic is locked on ") + (c)))
#define InvalidPort(p) (Message(-12, String((long)(p)) + " is an invalid port number"))
#define InvalidTime(t) (Message(-13, String((long)(t)) + " is an invalid time"))
#define CanNotChangeServer (Message(-14, "I cannot change server without loosing op on one of my channels"))
#define EmptyServerList (Message(-15, "Server list is empty"))
#define InvalidServerNumber(n) (Message(-16, String((long)(n)) + " is an invalid server number"))
#define InvalidParameters (Message(-17, "Invalid parameters"))
#define NotFound(w) (Message(-18, String("I can not find ") + (w)));
#define NotInUserlist(w) (Message(-19, (w) + " is not in my userlist"))
#define AlreadyInUserlist(m, mc) (Message(-21, (m) + " is already in userlist on channel(s) " + (mc)))
#define EmptyMessage (Message(-23, "Can not send an empty message"))
#define EmptyAddressee (Message(-24, "Can not send to nobody"))
#define NotToChannel (Message(-25, "Can not send to a channel. Use \"say\" instead."))
#define NotConnected (Message(-26, "Not connected."))

#define CHECK_CONNECTION if (!bot->serverConnection) return NotConnected

Message
Commands::Action(Bot *bot, String channel, String message)
{
  CHECK_CONNECTION;

  if (!CHANNEL(channel))
    return NotOnChannel(channel);

  if (message.length() == 0)
    return InvalidParameters;

  QUEUE->sendPrivmsg(channel, String("\001ACTION ") +
                     message + "\001");

  return Ok;
}

Message
Commands::AddUser(Bot *bot, String who, String maskChannel, int level, 
		  int prot, String nick, long flags, String password)
{
  if (who.length() == 0 ||
      maskChannel.length() == 0 ||
      level < 0 ||
      level > User::MANAGER ||
      prot < 0 ||
      prot > User::NO_DEOP)
    return InvalidParameters;

  String mask;

  if (!Utils::isWildcard(who)) {
    mask = bot->getUserhost("", who);
    if (mask.length() == 0)
      return NotFound(who);
    mask = Utils::makeWildcard(mask);
  }

  if (bot->userList->isInUserList(mask, maskChannel))
    return AlreadyInUserlist(mask, maskChannel);

  bot->userList->addUser(mask, maskChannel, level, prot, nick, flags, -1, password);
  bot->rehash();

  return Ok;
}


Message
Commands::AddServer(Bot *bot, String servername, int port)
{
  if (port <= 0)
    return InvalidPort(port);

  bot->serverList->addServer(new class Server(servername, port));

  return Ok;
}


Message
Commands::Ban(Bot *bot, String channel, String who)
{
  CHECK_CONNECTION;

  Channel *c = CHANNEL(channel);

  if (!c)
    return NotOnChannel(channel);

  if (!bot->iAmOp(channel))
    return NotChannelOp(channel);

  String dest;

  if (!Utils::isWildcard(who))
    dest = bot->getUserhost(channel, who);
  else
    dest = who;

  if (dest.length() == 0)
    return NotFound(who);

  dest = Utils::makeWildcard(dest);
  Mask m(dest);

  for (list<UserListItem *>::iterator it = bot->userList->l.begin();
       it != bot->userList->l.end();
       it++)
    if (m.matches((*it)->mask) &&
        (*it)->channelMask.matches(channel) &&
        (*it)->prot >= User::NO_BAN)
      return UserProtected(who, channel);

  for (vector<BanEntry *>::iterator it = c->channelBanlist.begin();
       it != c->channelBanlist.end(); ++it)
    if (m.matches((*it)->banMask) && (*it)->banMask.getMask() != m.getMask())
      QUEUE->sendChannelMode(channel, "-b", (*it)->banMask.getMask());
  
  QUEUE->sendChannelMode(channel, "+b", dest);

  return Ok;
}

Message Commands::Cycle(Bot *bot, String channel, String reason)
{
   CHECK_CONNECTION;
   
   if (!CHANNEL(channel))
     return NotOnChannel(channel);
   
   if (reason == "")
     QUEUE->sendPart(channel, String("Cycle"));
   else
     QUEUE->sendPart(channel, String("[Cycle] ") + reason);
     
   QUEUE->sendJoin(channel, bot->wantedChannels[channel]->key);
   
   return Ok;
}

Message
Commands::Deban(Bot *bot, String channel, String who)
{
  CHECK_CONNECTION;

  Channel *c = CHANNEL(channel);

  if (!c)
    return NotOnChannel(channel);

  if (!bot->iAmOp(channel))
    return NotChannelOp(channel);

  String dest;

  if (!Utils::isWildcard(who))
    dest = bot->getUserhost(channel, who);
  else
    dest = who;

  if (dest.length() == 0)
    return UserNotFound(who, channel);

  dest = Utils::makeWildcard(dest);
  Mask m(dest);
  
  for (vector<BanEntry *>::iterator it = c->channelBanlist.begin();
       it != c->channelBanlist.end(); ++it)
    if (m.matches((*it)->getMask())) {
       QUEUE->sendChannelMode(channel, "-b", (*it)->getMask());
    }

  return Ok;
}

Message
Commands::DelServer(Bot *bot, int number)
{
  if (number < 0 || number >= bot->serverList->size())
    return InvalidServerNumber(number);

  bot->serverList->delServer(number);

  return Ok;
}

Message
Commands::DelUser(Bot *bot, String who, String maskChannel)
{
  if (who.length() == 0 || maskChannel.length() == 0)
    return InvalidParameters;

  String dest;

  if (!Utils::isWildcard(who)) {
    dest = bot->getUserhost("", who);
    if (dest.length() == 0)
      return NotFound(who);
    dest = Utils::makeWildcard(who);
  }

  if (!bot->userList->isInUserList(dest, maskChannel))
    return NotInUserlist(who);

  bot->userList->removeUser(dest, maskChannel);
  bot->rehash();

  return Ok;
}

Message
Commands::Deop(Bot *bot, String channel, String who)
{
  CHECK_CONNECTION;

  Channel *c = CHANNEL(channel);

  if (!c)
    return NotOnChannel(channel);

  if (!bot->iAmOp(channel))
    return NotChannelOp(channel);

  if (!Utils::isWildcard(who)) {
    User *u = c->getUser(who);
    if (!u)
      return UserNotFound(who, channel);
    if (!(u->mode & User::OP_MODE))
      return UserNotOp(who, channel);
    if (u->getProt() >= User::NO_DEOP)
      return UserProtected(who, channel);
    QUEUE->sendChannelMode(channel, "-o", who);
  } else {
    Mask m(who);
    for (map<String, User *, less<String> >::iterator
           it = c->channelMemory.begin();
         it != c->channelMemory.end(); ++it) {
      if (m.matches((*it).second->nick + "!" +
                    (*it).second->userhost) &&
          (*it).second->getProt() < User::NO_DEOP &&
          ((*it).second->mode & User::OP_MODE))
        QUEUE->sendChannelMode(channel, "-o", (*it).second->nick);
    }
  }

  return Ok;
}

Message
Commands::Die(Bot *bot, String reason)
{
  CHECK_CONNECTION;

  QUEUE->sendQuit(reason);
  bot->stop = true;

  return Ok;
}

Message
Commands::Do(Bot *bot, String command)
{
  CHECK_CONNECTION;

  QUEUE->addLine(command, 0, 0, ServerQueueItem::OTHER);
  return Ok;
}

Message
Commands::Invite(Bot *bot, String channel, String who)
{
  CHECK_CONNECTION;

  if (!bot->iAmOp(channel))
    return NotChannelOp(channel);

  QUEUE->sendInvite(channel, who);

  return Ok;
}

Message
Commands::Join(Bot *bot, String channel, String key)
{
  CHECK_CONNECTION;

  if (!Utils::isValidChannelName(channel))
    return InvalidChannel(channel);

  // We change the key only if we are not on the channel.
  // We don't trust the user...
  if (!CHANNEL(channel)) {
    if (bot->wantedChannels[channel])
      bot->wantedChannels[channel]->key = key;
    else {
      bot->wantedChannels[channel] = new wantedChannel("", "", key, 0, 0, 
						       "", "");
    }
  }
  QUEUE->sendJoin(channel, bot->wantedChannels[channel]->key);

  return Ok;
}

Message
Commands::Keep(Bot *bot, String channel, String modes)
{
  CHECK_CONNECTION;

  Channel *c = CHANNEL(channel);

  if (!c)
    return NotOnChannel(channel);

  c->keepModes = modes;

  return Ok;  
}

Message
Commands::Kick(Bot *bot, String channel, String who, String reason)
{
  CHECK_CONNECTION;

  Channel *c = CHANNEL(channel);

  if (!c)
    return NotOnChannel(channel);

  if (!bot->iAmOp(channel))
    return NotChannelOp(channel);

  if (Utils::isWildcard(who)) {
    Mask m(who);
    for (map<String, User *, less<String> >::iterator it =
           c->channelMemory.begin();
         it != c->channelMemory.end();
         ++it)
      if (m.matches((*it).second->nick + "!" +
                    (*it).second->userhost) &&
          (*it).second->getProt() < User::NO_KICK)
        QUEUE->sendKick(channel, (*it).second->nick, reason);
  } else {
    User * u = c->getUser(who);
    if (!u)
      return UserNotFound(who, channel);
    if (u->getProt() < User::NO_KICK)
      QUEUE->sendKick(channel, who, reason);
    else
      return UserProtected(who, channel);
  }

  return Ok;
}

Message
Commands::KickBan(Bot *bot, String channel, String who, String reason)
{
  CHECK_CONNECTION;

  Message m = Commands::Ban(bot, channel, who);

  if (m.getCode() == 0)
    m = Commands::Kick(bot, channel, who, reason);

  return m;
}

Message
Commands::Lock(Bot *bot, String channel)
{
  CHECK_CONNECTION;

  Channel *c = CHANNEL(channel);

  if (!c)
    return NotOnChannel(channel);

  c->lockedTopic = true;

  return Ok;
}

Message
Commands::Mode(Bot *bot, String channel, String mode)
{
  CHECK_CONNECTION;

  if (!CHANNEL(channel))
    return NotOnChannel(channel);

  if (!bot->iAmOp(channel))
    return NotChannelOp(channel);

  QUEUE->sendChannelMode(String("MODE ") + channel + " " + mode);

  return Ok;
}

Message
Commands::Msg(Bot *bot, String who, String message)
{
  CHECK_CONNECTION;

  if (who == "")
    return EmptyAddressee;

  if (Utils::isChannel(who))
    return NotToChannel;

  if (message == "")
    return EmptyMessage;

  QUEUE->sendPrivmsg(who, message);

  return Ok;
}

Message
Commands::NextServer(Bot *bot)
{
  CHECK_CONNECTION;

  if (bot->serverList->size() == 0)
    return EmptyServerList;

  if (!bot->canChangeServer())
    return CanNotChangeServer;
  
  QUEUE->sendQuit("Changing server");
  bot->nextServer();

  return Ok;
}

Message
Commands::Nick(Bot *bot, String nick)
{
  CHECK_CONNECTION;

  if (nick == "" || !Utils::isValidNickName(nick))
    return InvalidNick(nick);
  
  bot->wantedNickName = nick;
  QUEUE->sendNick(nick);

  return Ok;
}


Message
Commands::Op(Bot *bot, String channel, String who)
{
  CHECK_CONNECTION;

  Channel *c = CHANNEL(channel);

  if (!c)
    return NotOnChannel(channel);

  if (!bot->iAmOp(channel))
    return NotChannelOp(channel);

  if (Utils::isWildcard(who))
    return MassOpNotAllowed;

  User *u = c->getUser(who);
  if (!u)
    return UserNotFound(who, channel);

  QUEUE->sendChannelMode(channel, "+o", who);

  return Ok;
}


Message
Commands::Part(Bot *bot, String channel, String reason)
{
  CHECK_CONNECTION;

  if (!CHANNEL(channel))
    return NotOnChannel(channel);

  wantedChannel *w = bot->wantedChannels[channel];
  bot->wantedChannels.erase(channel);
  delete w;
   
  QUEUE->sendPart(channel, reason);

  return Ok;
}

Message
Commands::Reconnect(Bot *bot)
{
  CHECK_CONNECTION;

  if (!bot->canChangeServer())
    return CanNotChangeServer;

  QUEUE->sendQuit("Reconnecting");
  bot->reconnect();

  return Ok;
}

Message
Commands::Say(Bot *bot, String channel, String message)
{
  CHECK_CONNECTION;

//   if (!CHANNEL(channel))
//     return NotOnChannel(channel);

  QUEUE->sendPrivmsg(channel, message);

  return Ok;
}


Message
Commands::Server(Bot *bot, int number)
{
  CHECK_CONNECTION;

  if (number < 0 || number >= bot->serverList->size())
    return InvalidServerNumber(number);

  if (!bot->canChangeServer())
    return CanNotChangeServer;

  QUEUE->sendQuit("Changing server");
  QUEUE->flush();
  bot->connect(number);

  return Ok;
}

Message
Commands::TBan(Bot *bot, String channel, String who, int seconds)
{
  CHECK_CONNECTION;

  Channel *c = CHANNEL(channel);

  if (!c)
    return NotOnChannel(channel);

  if (!bot->iAmOp(channel))
    return NotChannelOp(channel);

  if (seconds <= 0)
    return InvalidTime(seconds);

  String dest;

  if (!Utils::isWildcard(who))
    dest = bot->getUserhost(channel, who);
  else
    dest = who;

  if (dest.length() == 0)
    return UserNotFound(who, channel);

  dest = Utils::makeWildcard(dest);
  Mask m(dest);

  for (list<UserListItem *>::iterator it = bot->userList->l.begin();
       it != bot->userList->l.end();
       it++)
    if (m.matches((*it)->mask) &&
        (*it)->channelMask.matches(channel) &&
        (*it)->prot >= User::NO_BAN)
      return UserProtected(who, channel);

  for (vector<BanEntry *>::iterator it = c->channelBanlist.begin();
       it != c->channelBanlist.end(); ++it)
    if (m.matches((*it)->banMask))
      QUEUE->sendChannelMode(channel, "-b", (*it)->banMask.getMask());

  CHANNEL(channel)->addBan(dest, seconds);
  QUEUE->sendChannelMode(channel, "+b", dest);
  bot->todoList->addDeban(channel, dest, seconds);
}


Message
Commands::TKBan(Bot *bot, String channel, String who, int seconds, String reason)
{
  CHECK_CONNECTION;

  Message m = Commands::TBan(bot, channel, who, seconds);

  if (m.getCode() == 0)
    m = Commands::Kick(bot, channel, who, reason);

  return m;
}


Message
Commands::Topic(Bot *bot, String channel, String topic)
{
  CHECK_CONNECTION;

  Channel *c = CHANNEL(channel);

  if (!c)
    return NotOnChannel(channel);

  if (!bot->iAmOp(channel) && !(c->channelMode & Channel::TOPIC_RESTRICTED))
    return CanNotChangeTopic(channel);

  if (c->lockedTopic)
    return TopicLocked(channel);

  QUEUE->sendTopic(channel, topic);

  return Ok;
}


Message
Commands::Unlock(Bot *bot, String channel)
{
  CHECK_CONNECTION;

  Channel *c = CHANNEL(channel);

  if (!c)
    return NotOnChannel(channel);

  c->lockedTopic = false;

  return Ok;
}
