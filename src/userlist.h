#ifndef __USERLIST_H_
# define __USERLIST_H_

# include <time.h>
# include <list.h>

# include "string.h"
# include "userlistitem.h"

class Commands;
class UserCommands;

class UserList {
   String listFilename;
   list<UserListItem *> l;
   
 public:
   UserList(String);
   ~UserList();
   
   void read();
   void save();
   void clear();
   
   void addUser(String, String, int, int, String,
		long = 0, time_t = -1, String = "");
   void addUserFirst(String, String, int, int, String,
		     long = 0, time_t = -1, String = "");
   void removeFirst();
   void removeUser(String, String);
   
   UserListItem * getUserListItem(String, String);
   
   int getMaxLevel(String);
   int getLevel(String, String);
   
   int getMaxProt(String, String);
   
   bool isInUserList(String, String = "");
   
   friend class Commands;
   friend class UserCommands;
};

#endif
