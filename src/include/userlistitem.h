#ifndef __USERLISTITEM_H_
# define __USERLISTITEM_H_

# include <time.h>

# include "mask.h"

class User;
class Utils;
class UserList;
class Commands;

class UserListItem {
   // Mask on the user
   Mask mask;
   // Mask on the channel
   Mask channelMask; // LEGACY
   // Time the user was registered with the bot
   time_t registered;
   // Level (from 0 to 4)
   int level;
   // Protection (from 0 to 2)
   int prot;
   // Nickname
   String nick;
   // 32-bits of flags (flags.h)
   unsigned long flags;
   // Date last seen
   time_t lastseen;
   // Password
   String passwd;
   // How many channels are we identified on ?
   bool identified;
   
 public:
   UserListItem(String m, time_t r, int l, int p, String n = "",
		long f = 0, time_t ls = -1, String pass = "")
     : mask(m), channelMask("#*"), registered(r), level(l), prot(p), nick(n),
   flags(f), lastseen(ls), passwd(pass), identified(0) { }
   
   // Returns true if it matches <m> on channel(s) <mc>
   bool matches(String m, String mc)
     { if (mc.length()==0) return mask.matches(m);
	else return mask.matches(m) && channelMask.matches(mc); }
   
   // Returns true if it matches <m>
   bool matches(String m)
     { return mask.matches(m); }
   
   friend class User;
   friend class Utils;
   friend class Parser;
   friend class Channel;
   friend class UserList;
   friend class Commands;
};

#endif
