
#ifndef __SERVERQUEUEITEM_H_
# define __SERVERQUEUEITEM_H_

class ServerQueue;

# include "serverqueue.h"

class ServerQueueItem {
 private:
   ServerQueueItem(const ServerQueueItem &);
   
 public:
   int priority; 
   int penalty; 
   int type;
   
   enum {
      CHANNELMODE, 
      INVITE,
      ISON,
      JOIN, 
      KICK,
      NICK, 
      NOTICE, 
      OTHER,
      PART, 
      PASS, 
      PING, 
      PONG, 
      PRIVMSG,
      QUIT, 
      TOPIC, 
      USER, 
      USERHOST, 
      USERMODE, 
      WHO, 
      WHOIS 
   };
   
   ServerQueueItem(int, int, int);
   virtual ~ServerQueueItem() { }
   
   virtual bool merge(ServerQueueItem *) { return false; }
   virtual String getLine() = 0;
   
   friend class ServerQueue;
};


class ServerQueueChannelModeItem : public ServerQueueItem {
 public:
   String channel;
   String mode;
   String parameters;
   int paramcount;
   
   ServerQueueChannelModeItem(String, String, String);
   bool merge(ServerQueueItem *);
   String getLine();
};

class ServerQueueIsonItem : public ServerQueueItem {
 public:
   String nick;

   ServerQueueIsonItem(String);
   bool merge(ServerQueueItem *);
   String getLine();
};

class ServerQueueJoinItem : public ServerQueueItem {
 public:
   String param; // includes channel, may also include ' '+key.
   int count;
   
   ServerQueueJoinItem(String);
   bool merge(ServerQueueItem *);
   String getLine();
};

class ServerQueueKickItem : public ServerQueueItem {
 public:
   String channel;
   String who;
   String reason;
   int count;
   
   ServerQueueKickItem(String, String, String);
   bool merge(ServerQueueItem *);
   String getLine();
};

class ServerQueueNoticeItem : public ServerQueueItem {
 public:
   String dest;
   String message;
   int count;
   
   ServerQueueNoticeItem(String, String);
   bool merge(ServerQueueItem *);
   String getLine();
};

class ServerQueueOtherItem : public ServerQueueItem {
 public:
   String line;
   
   ServerQueueOtherItem(String, int, int, int);
   String getLine();
};

class ServerQueueWhoItem : public ServerQueueItem {
 public:
   String subject; 
   int count;
   
   ServerQueueWhoItem(String);
   bool merge(ServerQueueItem *);
   String getLine();
};

class ServerQueueUserhostItem : public ServerQueueItem {
 public:
   String nick;
   int count;
   
   ServerQueueUserhostItem(String);
   bool merge(ServerQueueItem *);
   String getLine();
};

#endif
