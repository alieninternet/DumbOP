
#ifndef __SERVERCONNECTION_H_
#define __SERVERCONNECTION_H_

#include <time.h>
#include <sys/timeb.h>

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
   Bot * bot; // Recursive

   long receivedLen;
   long sentLen;
   
  ServerQueue * queue;
  struct timeb pingTime;
  long long lag;
#ifdef DEBUG
   bool debug;
#endif
  time_t serverLastSpoken;
  
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
   friend class ServerQueue;
   friend class Games;
   friend class GameQuiz;
};

#endif
