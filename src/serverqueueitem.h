
#ifndef __SERVERQUEUEITEM_H_
#define __SERVERQUEUEITEM_H_

class ServerQueueItem {
public:
  int priority, penalty, type;

  enum {
    CHANNELMODE, USERMODE, INVITE, JOIN, KICK,
    NICK, NOTICE, PART, PASS, PING, PONG, PRIVMSG,
    QUIT, TOPIC, USER, USERHOST, WHO, WHOIS, OTHER
  };

  ServerQueueItem(int, int, int);
  virtual ~ServerQueueItem() { }

  virtual bool merge(ServerQueueItem *) { return false; }
  virtual String getLine() = 0;

  friend class ServerQueue;

private:
  ServerQueueItem(const ServerQueueItem &);
};


class ServerQueueOtherItem : public ServerQueueItem {
public:
  String line;

  ServerQueueOtherItem(String, int, int, int);
  String getLine();
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

#endif
