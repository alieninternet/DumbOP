#ifndef __USERLIST_H_
# define __USERLIST_H_

# include <time.h>
# include <list.h>

# include "strings.h"
# include "userlistitem.h"

class Commands;

class UserList {
   String listFilename;
   list<UserListItem *> l;
   
 public:
   UserList(String);
   ~UserList();
   
   void read();
   void save();
   void clear();
   
   void addUser(String, time_t, int, unsigned long, String,
		long = 0, time_t = -1, String = "");
   void removeUser(String);

   UserListItem *getUserListItem(String);
   UserListItem *getUserListItemNick(String);
   
   int getMaxLevel(String);
   int getLevel(String);
   
   bool isInUserList(String);
   bool isNickInUserList(String);

   bool identify(String, String, bool = true);

   friend class Commands;
};

#endif
