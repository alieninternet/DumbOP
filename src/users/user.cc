
#include "utils.h"
#include "user.h"

User::User(String n, String uh, int mode, UserList *ul)
  : mode(mode), floodNum(0),
    userListItem(ul->getUserListItem(n + "!" + uh)),
    nick(n), userhost(uh),
    userkey("")
{
}

User::User(String n, int mode)
  : mode(mode), floodNum(0),
    userListItem(0), nick(n), userhost("")
{ 
}

int User::getLevel()
{
   if (userListItem && userListItem->identified)
     return userListItem->level;
   return 0;
}

int User::getProt()
{
//   if (userListItem && userListItem->identified)
//     return userListItem->prot;
   return 0;
}

bool User::getAop()
{
//  if (userListItem && userListItem->identified)
//    return userListItem->aop;
  return false;
}
