#ifndef __BOT_H_
#define __BOT_H_

#include <time.h>
#include <set.h>
#include <fstream.h>

#include "string.h"
#include "person.h"
#include "serverlist.h"
#include "channellist.h"
#include "userlist.h"
#include "todolist.h"
#include "parser.h"

#define VERSION_STRING   "DumbOP 0.29; <pickle@alien.net.au>"
#define COPYRIGHT_STRING "Copyright (c) Simon Butcher <pickle@alien.net.au>"

class Channel;
class DCCConnection;
class Parser;
class DCCParser;
class ServerConnection;
class Commands;
class UserCommands;
class Utils;

class Bot {
public:
  String nickName;
  String wantedNickName;
  String userName;
  String ircName;
  String userHost;
  String localIP;
  char commandChar;

  String configFileName;
  String userListFileName;
  String logFileName;
  String helpFileName;
  String initFileName;

  ofstream logFile;

  long receivedLen;
  long sentLen;
   
  bool connected;
  bool debug;
  bool stop;
  bool sentPing;
   
  ChannelList * channelList;
  UserList * userList;
  ServerList * serverList;
  TodoList * todoList;
  list<class userFunction *> userFunctions;

  map<String, wantedChannel *, less<String> > wantedChannels;

  map<String, unsigned int, less<String> > ignoredUserhosts;
  map<String, Person *, less<String> > spyList;

  time_t startTime, currentTime, lastNickNameChange, lastChannelJoin;

  ServerConnection * serverConnection;
  list<DCCConnection *> dccConnections;

  map<unsigned long, String, less<unsigned long> > userhostMap;
  unsigned long sentUserhostID;
  unsigned long receivedUserhostID;

  static const unsigned int MAX_MESSAGES = 2;
  static const time_t NICK_CHANGE  = 60;
  static const time_t CHANNEL_JOIN = 600;
  static const time_t IGNORE_DELAY = 180;
  static const time_t DCC_DELAY = 300;
  static const time_t PING_TIME = 90;
  static const time_t TIMEOUT = 120;

public:

  Bot(String, bool=false);
  ~Bot();

  void run();

  void waitForInput();

  void logLine(String);
  void readConfig();

  void reconnect();

  friend class Parser;
  friend class DCCParser;
  friend class Person;
  friend class Channel;
  friend class Utils;
  friend class ServerConnection;
  friend class UserCommands;
  friend class Commands;
  
private:
  bool canChangeServer();
  void nextServer();
  void connect(int);

  void addDCC(Person *, unsigned long, int);

  void rehash();
  String getUserhost(String, String);
  bool iAmOp(String);
};

#endif
