
#ifndef __DCCCONNECTION_H_
#define __DCCCONNECTION_H_

#include <time.h>

#include "connection.h"
#include "bot.h"

class DCCPerson;
class UserCommands;

class DCCConnection : public Connection {
  Bot *bot;
  String nuh;
  time_t lastSpoken;
  bool autoRemove;

public:
  DCCConnection(Bot *, String, unsigned long, int);

  bool connect();
  bool handleInput();

  void sendNotice(String);

  friend class Bot;
  friend class DCCPerson;
  friend class DCCParser;
  friend class UserCommands;
};

#endif
