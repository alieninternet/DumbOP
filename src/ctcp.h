/* ctcp.h
 * Parse CTCP commands
 */

#ifndef __CTCP_H_
# define __CTCP_H_

# include "string.h"
# include "person.h"
# include "serverconnection.h"

class CTCP {
 public:
   static void Action(ServerConnection *, Person *, String);
   static void ClientInfo(ServerConnection *, Person *, String);
   static void DCC(ServerConnection *, Person *, String);
   static void Echo(ServerConnection *, Person *, String);
   static void ErrMsg(ServerConnection *, Person *, String);
   static void Finger(ServerConnection *, Person *, String);
   static void Lag(ServerConnection *, Person *, String);
   static void Ping(ServerConnection *, Person *, String);
   static void Sex(ServerConnection *, Person *, String);
   static void Source(ServerConnection *, Person *, String);
   static void Time(ServerConnection *, Person *, String);
   static void Uptime(ServerConnection *, Person *, String);
   static void UserInfo(ServerConnection *, Person *, String);
   static void Version(ServerConnection *, Person *, String);
};

#endif
