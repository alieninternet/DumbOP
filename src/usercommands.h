
#ifndef __USERCOMMANDS_H_
#define __USERCOMMANDS_H_

#include "string.h"
#include "person.h"
#include "serverconnection.h"

#ifdef USESCRIPTS
#include "Interp.H"
#endif

class UserCommands {
 public:
   static void Access(ServerConnection *, Person *, String, String);
   static void AddUser(ServerConnection *, Person *, String, String);
   static void AddServer(ServerConnection *, Person *, String, String);
   static void Alias(ServerConnection *, Person *, String, String);
   static void Ban(ServerConnection *, Person *, String, String);
   static void BanList(ServerConnection *, Person *, String, String);
   static void Beep(ServerConnection *, Person *, String, String);
   static void ChangeLevel(ServerConnection *, Person *, String, String);
   static void Cycle(ServerConnection *, Person *, String, String);
   static void DCCList(ServerConnection *, Person *, String, String);
   static void Deban(ServerConnection *, Person *, String, String);
   static void DelServer(ServerConnection *, Person *, String, String);
   static void DelUser(ServerConnection *, Person *, String, String);
   static void Deop(ServerConnection *, Person *, String, String);
   static void Die(ServerConnection *, Person *, String, String);
   static void Do(ServerConnection *, Person *, String, String);
   static void Help(ServerConnection *, Person *, String, String);
   static void Ident(ServerConnection *, Person *, String, String);
   static void Info(ServerConnection *, Person *, String, String);
   static void Invite(ServerConnection *, Person *, String, String);
   static void Join(ServerConnection *, Person *, String, String);
   static void Keep(ServerConnection *, Person *, String, String);
   static void Kick(ServerConnection *, Person *, String, String);
   static void KickBan(ServerConnection *, Person *, String, String);
   static void LastSeen(ServerConnection *, Person *, String, String);
   static void Mode(ServerConnection *, Person *, String, String);
   static void Msg(ServerConnection *, Person *, String, String);
   static void Names(ServerConnection *, Person *, String, String);
   static void NextServer(ServerConnection *, Person *, String, String);
   static void Nick(ServerConnection *, Person *, String, String);
   static void NsLookup(ServerConnection *, Person *, String, String);
   static void Op(ServerConnection *, Person *, String, String);
   static void Part(ServerConnection *, Person *, String, String);
   static void Password(ServerConnection *, Person *, String, String);
   static void Raw(ServerConnection *, Person *, String, String);
   static void Reconnect(ServerConnection *, Person *, String, String);
   static void RSpyMessage(ServerConnection *, Person *, String, String);
   static void Save(ServerConnection *, Person *, String, String);
   static void Say(ServerConnection *, Person *, String, String);
   static void Server(ServerConnection *, Person *, String, String);
   static void ServerList(ServerConnection *, Person *, String, String);
   static void Stats(ServerConnection *, Person *, String, String);
   static void Test(ServerConnection *, Person *, String, String);
   static void TBan(ServerConnection *, Person *, String, String);
   static void TKBan(ServerConnection *, Person *, String, String);
   static void Topic(ServerConnection *, Person *, String, String);
   static void UserList(ServerConnection *, Person *, String, String);
};

#endif
