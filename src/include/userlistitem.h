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
   // Time the user was registered with the bot
   time_t registered;
   // Level (from 0 to 4)
   int level;
   // Credits this user has accumulated (used in games)
   unsigned long credits;
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
   UserListItem(String, time_t, int, unsigned long, String, long, time_t,
		String);
   
   bool matches(String);			// Check for a mask match

   bool canAfford(unsigned long);		// Can afford amount?
   void charge(unsigned long);			// Charge user an amount
   void pay(unsigned long);			// Pay user an amount
   
   friend class User;
   friend class Utils;
   friend class Parser;
   friend class Channel;
   friend class UserList;
   friend class Commands;
   
   friend class GameQuiz;
   
   friend class TelnetDialogueLogin;
};

#endif
