
#ifndef __USER_H_
#define __USER_H_

#include "userlist.h"
#include "userlistitem.h"

class Bot;
class Utils;
class Parser;
class Channel;
class Commands;
class UserCommands;

class User {

  int mode;
  int floodNum;
  UserListItem * userListItem;
  String nick;
  String userhost;
  String userkey;

public:
  enum {
    OP_MODE = 1,
    VOICE_MODE = 2,
    AWAY_MODE = 4,
    IRCOP_MODE = 8
  };

  enum {
    NONE = 0,
    USER = 1,
    TRUSTED_USER = 2,
    FRIEND = 3,
    MASTER = 4
  };

  enum {
    NO_PROT = 0,
    NO_BAN = 1,
    NO_KICK = 2,
    NO_DEOP = 3
  };

  User(String, String, String, int, UserList *);
  User(String, int);
  
  int getLevel();
  int getProt();
  bool getAop();

  friend class Bot;
  friend class Utils;
  friend class Parser;
  friend class Channel;
  friend class Commands;
  friend class UserCommands;
};

#endif
