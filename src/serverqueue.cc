
#include <limits.h>
#include "serverqueue.h"

#ifdef DEBUG
ServerQueue::ServerQueue(ServerConnection *c, Socket *s, bool d)
: cnx(c),
  Queue(s,d), 
  penalty(0)
#else
ServerQueue::ServerQueue(ServerConnection *c, Socket *s)
: cnx(c),
  Queue(s), 
  penalty(0)
#endif
{
  serverQueue.clear();
}

ServerQueue::~ServerQueue()
{
  penalty = INT_MIN;
  flush();
}

void
ServerQueue::addItem(ServerQueueItem *sqi)
{
   // We want to keep track of how much we are sending out
   cnx->sentLen += (sqi->getLine().length() - 1);
   
   list<ServerQueueItem *>::iterator it, it2;
   
   for (it = serverQueue.begin(); it != serverQueue.end(); ++it) {
      if ((*it)->priority > sqi->priority)
	break;
   }
   it2 = it; --it2;
   if (it2 != serverQueue.end() && *it2) {
      // try to merge this item to the previous
      if ((*it2)->merge(sqi)) {
	 delete sqi;
	 return;
      }
   }
   serverQueue.insert(it, sqi);
}

void
ServerQueue::addLine(String l, int pr, int pen, int t)
{
   ServerQueueOtherItem * sqoi =
     new ServerQueueOtherItem(l, pr, pen, t);
   addItem(sqoi);
}

bool
ServerQueue::flush()
{
  // Called every second, we decrement the penalty
  if (penalty > 0)
    penalty--;

  while (!serverQueue.empty() && (penalty < 5)) {
    ServerQueueItem * sqi = (*serverQueue.begin());
    penalty += sqi->penalty + sqi->getLine().length()/100 + 1;
    bool res = sendLine(sqi->getLine());
    serverQueue.erase(serverQueue.begin());
    delete sqi;
    if (!res)
      return false;
  }
  return true;
}

void
ServerQueue::sendCTCP(String to, String command,
                      String message)
{
  sendPrivmsg(to, String("\001") + command + " " + message + "\001");
}

void
ServerQueue::sendCTCPReply(String to, String command,
			   String message)
{
  sendNotice(to, String("\001") + command + " " + message + "\001");
}

void
ServerQueue::sendChannelMode(String mode)
{
   addLine(mode, CHANNELMODE_PRIORITY, CHANNELMODE_PENALTY,
	   ServerQueueItem::CHANNELMODE);
}

void
ServerQueue::sendChannelMode(String channel, String mode, String parameters)
{
  ServerQueueChannelModeItem * sqcmi =
    new ServerQueueChannelModeItem(channel, mode, parameters);
  addItem(sqcmi);
}

void
ServerQueue::sendInvite(String channel, String nick)
{
  addLine(String("INVITE ") + nick + " " + channel,
          INVITE_PRIORITY, INVITE_PENALTY, ServerQueueItem::INVITE);
}

void
ServerQueue::sendJoin(String channel, String key)
{
  addLine(String("JOIN ") + channel + " " + key,
          JOIN_PRIORITY, JOIN_PENALTY, ServerQueueItem::JOIN);
}

void
ServerQueue::sendKick(String channel, String nick, String reason)
{
  ServerQueueKickItem * sqki =
    new ServerQueueKickItem(channel, nick, reason);
  addItem(sqki);
}

void
ServerQueue::sendNick(String nick)
{
  addLine(String("NICK ") + nick,
          NICK_PRIORITY, NICK_PENALTY, ServerQueueItem::NICK);
}

void ServerQueue::sendNickopIdent(String password)
{
   addLine(String("PRIVMSG nickop@austnet.org :identify ") + password,
	   USERMODE_PRIORITY, USERMODE_PENALTY, ServerQueueItem::PRIVMSG);
}

void
ServerQueue::sendNotice(String to, String message)
{
  ServerQueueNoticeItem *sqni =
    new ServerQueueNoticeItem(to, message);
  addItem(sqni);
}

void ServerQueue::sendPart(String channel, String reason)
{
   addLine(String("PART ") + channel + 
	   ((reason == "") ?
	    String("") : (String(" :") + reason)),
	   PART_PRIORITY, PART_PENALTY, ServerQueueItem::PART);
}

void
ServerQueue::sendPass(String pass)
{
  addLine(String("PASS ") + pass,
          NICK_PRIORITY, NICK_PENALTY, ServerQueueItem::PASS);
}

void
ServerQueue::sendPing(String server)
{
  addLine(String("PING :") + server,
          PING_PRIORITY, PING_PENALTY, ServerQueueItem::PING);
}

void
ServerQueue::sendPong(String crap)
{
  addLine(String("PONG ") + crap,
          PONG_PRIORITY, PONG_PENALTY, ServerQueueItem::PONG);
}

void
ServerQueue::sendPrivmsg(String dest, String message)
{
  addLine(String("PRIVMSG ") + dest + " :" + message,
          PRIVMSG_PRIORITY, PRIVMSG_PENALTY, ServerQueueItem::PRIVMSG);
}

void
ServerQueue::sendQuit(String reason)
{
  addLine(String("QUIT :") + reason,
          QUIT_PRIORITY, QUIT_PENALTY, ServerQueueItem::QUIT);
}

void
ServerQueue::sendTopic(String channel, String topic)
{
  addLine(String("TOPIC ") + channel + " :" + topic,
          TOPIC_PRIORITY, TOPIC_PENALTY, ServerQueueItem::TOPIC);
}

void
ServerQueue::sendUser(String username, String ircname)
{
  addLine(String("USER ") + username + " . . :" + ircname,
          NICK_PRIORITY, NICK_PENALTY, ServerQueueItem::USER);
}

void
ServerQueue::sendUserMode(String nick, String mode)
{
  addLine(String("MODE ") + nick + " " + mode,
          USERMODE_PRIORITY, USERMODE_PENALTY,
          ServerQueueItem::USERMODE);
}

void
ServerQueue::sendUserhost(String nick)
{
  addLine(String("USERHOST ") + nick,
          USERHOST_PRIORITY, USERHOST_PENALTY, ServerQueueItem::USERHOST);
}

void
ServerQueue::sendWho(String who)
{
  addLine(String("WHO ") + who,
          WHO_PRIORITY, WHO_PENALTY, ServerQueueItem::WHO);
}

void
ServerQueue::sendWhois(String nick)
{
  addLine(String("WHOIS ") + nick,
          NICK_PRIORITY, WHOIS_PENALTY, ServerQueueItem::WHOIS);
}
