
#ifndef __CHANNEL_H_
#define __CHANNEL_H_

#include <time.h>
#include <map.h>
#include <vector.h>

#include "user.h"
#include "person.h"
#include "serverconnection.h"
#include "banentry.h"

class Bot;
class Parser;
class Commands;

#define DEFAULT_KEEPMODES "iklmnpst"

// This struct is used to keep information about
// the channel we want to join, the modes we
// want to set/keep on these channels, and the
// channel keys
struct wantedChannel {
   String mode;
   String keep;
   String key;
   unsigned long flags;
   unsigned long gameflags;

   String greeting;			// Welcoming string
   String rules;			// Rules line
   
   wantedChannel(String m, String kp, String ky,
		 unsigned long f, unsigned long gf, String g, String r)
     : mode(m), keep(kp), key(ky), flags(f), gameflags(gf), 
       greeting(g), rules(r) { }
};

class Channel {
  String channelName;
  String channelTopic;
  bool lockedTopic;
  int channelMode;
  int channelLimit;
  String channelKey;
  String keepModes;
  String wantedModes;
  
  int count;
  int countOp;
  int countVoice;
  int count_peak;
  int countOp_peak;
  int countVoice_peak;
  bool joined;
  bool doMode;
  bool gotWho;
  map<String, User *, less<String> > channelMemory;
  vector<BanEntry *> channelBanlist;
  ServerConnection * cnx;

public:
  
  enum {
    PRIVATE = 1,            // +p
    SECRET = 2,             // +s
    INVITE_ONLY = 4,        // +i
    TOPIC_RESTRICTED = 8,   // +t
    EXTMSG_RESTRICTED = 16, // +n
    MODERATED = 32,         // +m
    HAS_KEY = 64,           // +k <key>
    IS_LIMITED = 128        // +l <limit>
  };
  
  Channel(ServerConnection *, String, String);
  ~Channel();

  void addNick(String, String, int, UserList *, bool = false);
  void delNick(String);
  void changeNick(String, String);
  bool hasNick(String);
  User * getUser(String);
  
  void addBan(String, time_t = -1);
  void delBan(String);
  
  void parseMode(Person *, String);
  void resynchModes();

  void sendNotice(String);
   
   friend class Bot;
   friend class Parser;
   friend class Commands;
   friend class Games;
   friend class GameQuiz;
};

#endif
