
#ifndef __SERVERQUEUE_H_
# define __SERVERQUEUE_H_

# include <list.h>

# include "strings.h"
# include "queue.h"

class ServerConnection;
class ServerQueueItem;

# include "serverconnection.h"
# include "serverqueueitem.h"

# define IRC_MODE_WALLOPS	0x0004	// Bit 2 = +w (RFC2812)
# define IRC_MODE_INVISIBLE	0x0008	// Bit 3 = +i (RFC2812)

enum {
   QUIT_PRIORITY,
   USERMODE_PRIORITY,
   CHANNELMODE_PRIORITY,
   KICK_PRIORITY,
   PONG_PRIORITY,
   TOPIC_PRIORITY,
   PART_PRIORITY,
   NICK_PRIORITY,
   ISON_PRIORITY,
   USERHOST_PRIORITY,
   WHO_PRIORITY,
   JOIN_PRIORITY,
   PING_PRIORITY,
   INVITE_PRIORITY,
   PRIVMSG_PRIORITY,
   NOTICE_PRIORITY
};

static const int CHANNELMODE_PENALTY = 2;
static const int INVITE_PENALTY = 1;
static const int ISON_PENALTY = 1;
static const int JOIN_PENALTY = 1;
static const int KICK_PENALTY = 2;
static const int NICK_PENALTY = 1;
static const int NOTICE_PENALTY = 1;
static const int PART_PENALTY = 1;
static const int PING_PENALTY = 1;
static const int PONG_PENALTY = 1;
static const int PRIVMSG_PENALTY = 1;
static const int QUIT_PENALTY = 1;
static const int TOPIC_PENALTY = 2;
static const int USERHOST_PENALTY = 1;
static const int USERMODE_PENALTY = 2;
static const int WHO_PENALTY = 4;
static const int WHOIS_PENALTY = 1;



class ServerQueue : public Queue {
   list<ServerQueueItem *> serverQueue;
   ServerConnection *cnx;
   int penalty;
   
 public:
   ServerQueue(ServerConnection *, Socket *);
   ~ServerQueue();
   
   void addItem(ServerQueueItem *);
   void addLine(String, int, int, int);
   bool flush();
   
   void sendCTCP(String, String, String);
   void sendCTCPReply(String, String, String);
   void sendChannelMode(String);
   void sendChannelMode(String, String, String);
   void sendInvite(String, String);
   void sendIson(String);
   void sendJoin(String, String);
   void sendKick(String, String, String);
   void sendNick(String);
   void sendNickopIdent(String);
   void sendNotice(String, String);
   void sendPart(String, String);
   void sendPass(String);
   void sendPing(String);
   void sendPong(String);
   void sendPrivmsg(String, String);
   void sendQuit(String);
   void sendTopic(String, String);
   void sendUser(String, String, unsigned char);
   void sendUserMode(String, String);
   void sendUserhost(String);
   void sendWho(String);
   void sendWhois(String);
};

#endif
