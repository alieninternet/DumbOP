
#include "serverqueue.h"

void ServerQueue::sendCTCP(String to, String command,
			   String message)
{
   sendPrivmsg(to, String("\001") + command + " " + message + "\001");
}

void ServerQueue::sendCTCPReply(String to, String command,
				String message)
{
   sendNotice(to, String("\001") + command + " " + message + "\001");
}

void ServerQueue::sendChannelMode(String mode)
{
   addLine(mode, CHANNELMODE_PRIORITY, CHANNELMODE_PENALTY,
	   ServerQueueItem::CHANNELMODE);
}

void ServerQueue::sendChannelMode(String channel, String mode, String parameters)
{
   ServerQueueChannelModeItem * sqcmi =
     new ServerQueueChannelModeItem(channel, mode, parameters);
   addItem(sqcmi);
}

void ServerQueue::sendInvite(String channel, String nick)
{
   addLine(String("INVITE ") + nick + " " + channel,
	   INVITE_PRIORITY, INVITE_PENALTY, ServerQueueItem::INVITE);
}

void ServerQueue::sendIson(String nick)
{
   ServerQueueIsonItem *sqii =
     new ServerQueueIsonItem(nick);
   addItem(sqii);
}

void ServerQueue::sendJoin(String channel, String key)
{
   ServerQueueJoinItem *sqji =
     new ServerQueueJoinItem(channel + ((key.length()) ?
					(String(" ") + key) :
					String("")));
   addItem(sqji);
}

void ServerQueue::sendKick(String channel, String nick, String reason)
{
   ServerQueueKickItem * sqki =
     new ServerQueueKickItem(channel, nick, reason);
   addItem(sqki);
}

void ServerQueue::sendNick(String nick)
{
   addLine(String("NICK ") + nick,
	   NICK_PRIORITY, NICK_PENALTY, ServerQueueItem::NICK);
}

void ServerQueue::sendNickopIdent(String password)
{
   addLine(String("PRIVMSG nickop@austnet.org :identify ") + password,
	   USERMODE_PRIORITY, USERMODE_PENALTY, ServerQueueItem::PRIVMSG);
}

void ServerQueue::sendNotice(String to, String message)
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

void ServerQueue::sendPass(String pass)
{
   addLine(String("PASS ") + pass,
	   NICK_PRIORITY, NICK_PENALTY, ServerQueueItem::PASS);
}

void ServerQueue::sendPing(String server)
{
   addLine(String("PING :") + server,
	   PING_PRIORITY, PING_PENALTY, ServerQueueItem::PING);
}

void ServerQueue::sendPong(String crap)
{
   addLine(String("PONG ") + crap,
	   PONG_PRIORITY, PONG_PENALTY, ServerQueueItem::PONG);
}

void ServerQueue::sendPrivmsg(String dest, String message)
{
   addLine(String("PRIVMSG ") + dest + " :" + message,
	   PRIVMSG_PRIORITY, PRIVMSG_PENALTY, ServerQueueItem::PRIVMSG);
}

void ServerQueue::sendQuit(String reason)
{
   addLine(String("QUIT :") + reason,
	   QUIT_PRIORITY, QUIT_PENALTY, ServerQueueItem::QUIT);
}

void ServerQueue::sendTopic(String channel, String topic)
{
   addLine(String("TOPIC ") + channel + " :" + topic,
	   TOPIC_PRIORITY, TOPIC_PENALTY, ServerQueueItem::TOPIC);
}

void ServerQueue::sendUser(String username, String ircname, 
			   unsigned char mode)
{
   addLine(String("USER ") + username + String(" ") + String(mode) +
	   String(" * :") + ircname, 
	   NICK_PRIORITY, NICK_PENALTY, ServerQueueItem::USER);
}

void ServerQueue::sendUserMode(String nick, String mode)
{
   addLine(String("MODE ") + nick + " " + mode,
	   USERMODE_PRIORITY, USERMODE_PENALTY,
	   ServerQueueItem::USERMODE);
}

void ServerQueue::sendUserhost(String nick)
{
   ServerQueueUserhostItem *squi =
     new ServerQueueUserhostItem(nick);
   addItem(squi);
}

void ServerQueue::sendWho(String who)
{
   ServerQueueWhoItem *sqwi =
     new ServerQueueWhoItem(who);
   addItem(sqwi);
}

void ServerQueue::sendWhois(String nick)
{
   addLine(String("WHOIS ") + nick,
	   NICK_PRIORITY, WHOIS_PENALTY, ServerQueueItem::WHOIS);
}
