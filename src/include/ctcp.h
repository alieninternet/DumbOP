/* ctcp.h
 * Parse CTCP commands
 */

#ifndef __CTCP_H_
# define __CTCP_H_

# include "strings.h"
# include "person.h"
# include "serverconnection.h"

class CTCP {
 public:
   static void Action(Person *, String);
   static void ClientInfo(Person *, String);
   static void DCC(Person *, String);
   static void Echo(Person *, String);
   static void ErrMsg(Person *, String);
   static void Finger(Person *, String);
   static void Lag(Person *, String);
   static void Ping(Person *, String);
   static void Sex(Person *, String);
   static void Source(Person *, String);
   static void Time(Person *, String);
   static void Uptime(Person *, String);
   static void UserInfo(Person *, String);
   static void Version(Person *, String);
};

#endif
