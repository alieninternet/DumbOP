
#ifndef __SERVERCONNECTION_H_
#define __SERVERCONNECTION_H_

#include <time.h>

#include "server.h"
#include "serverqueue.h"
#include "connection.h"

class Bot;
class Parser;
class Channel;
class Commands;
class UserCommands;

class ServerConnection : public Connection {
  Server * server;
  Bot * bot;  
  ServerQueue * queue;
  time_t pingTime;
  int lag;
  time_t serverLastSpoken;
  bool debug;
  
public:
  ServerConnection(Bot *, Server *, String);
  ~ServerConnection();

  bool connect();
  bool handleInput();

  friend class Bot;
  friend class Person;
  friend class Parser;
  friend class Channel;
  friend class Commands;
  friend class UserCommands;
  friend class CTCP;
  friend class Version;
};

#endif
