#ifndef __COMMANDS_H_
#define __COMMANDS_H_

#include "strings.h"
#include "message.h"
#include "bot.h"
#include "serverconnection.h"

class Commands {
public:
  static Message Action(Bot *, String, String);
  static Message AddUser(Bot *, String, String, int, int, String, time_t, String);
  static Message AddServer(Bot *, String, int);
  static Message AddServer(Bot *, String, String);
  static Message Ban(Bot *, String, String);
  static Message ChangeLevel(Bot *, String, String, int);
  static Message Cycle(Bot *, String, String);
  static Message Deban(Bot *, String, String);
  static Message DelServer(Bot *, int);
  static Message DelUser(Bot *, String, String);
  static Message Deop(Bot *, String, String);
  static Message Die(Bot *, String = "Leaving");
  static Message Do(Bot *, String);
  static Message Invite(Bot *, String, String);
  static Message Join(Bot *, String, String = "");
  static Message Keep(Bot *, String, String);
  static Message Kick(Bot *, String, String, String);
  static Message KickBan(Bot *, String, String, String);
  static Message Lock(Bot *, String);
  static Message Mode(Bot *, String, String);
  static Message Msg(Bot *, String, String);
  static Message NextServer(Bot *);
  static Message Nick(Bot *, String);
  static Message Op(Bot *, String, String);
  static Message Part(Bot *, String, String);
  static Message Reconnect(Bot *);
  static Message Say(Bot *, String, String);
  static Message Server(Bot *, int);
  static Message TBan(Bot *, String, String, int);
  static Message TKBan(Bot *, String, String, int, String);
  static Message Topic(Bot *, String, String);
  static Message Unlock(Bot *, String);
};

#endif
