#ifndef __COMMANDS_H_
# define __COMMANDS_H_

# include "strings.h"
# include "person.h"
# include "serverconnection.h"

class Commands {
 public:
   static void Access(Person *, String, String);
   static void AddServer(Person *, String, String);
   static void Alias(Person *, String, String);
   static void Answer(Person *, String, String);
   static void Ban(Person *, String, String);
   static void BanList(Person *, String, String);
   static void Beep(Person *, String, String);
   static void Category(Person *, String, String);
   static void ChangeLevel(Person *, String, String);
   static void Checkpoint(Person *, String, String);
   static void Clue(Person *, String, String);
   static void Cycle(Person *, String, String);
   static void DCCList(Person *, String, String);
   static void Deban(Person *, String, String);
   static void DelServer(Person *, String, String);
   static void Deop(Person *, String, String);
   static void Die(Person *, String, String);
   static void Do(Person *, String, String);
   static void Give(Person *, String, String);
   static void Help(Person *, String, String);
   static void Hint(Person *, String, String);
   static void Identify(Person *, String, String);
   static void Inventory(Person *, String, String);
   static void Invite(Person *, String, String);
   static void Join(Person *, String, String);
   static void Keep(Person *, String, String);
   static void Kick(Person *, String, String);
   static void LastSeen(Person *, String, String);
   static void Mode(Person *, String, String);
   static void Names(Person *, String, String);
   static void NextServer(Person *, String, String);
   static void Note(Person *, String, String);
   static void Op(Person *, String, String);
   static void Part(Person *, String, String);
   static void Password(Person *, String, String);
   static void Ping(Person *, String, String);
   static void Raw(Person *, String, String);
   static void Reconnect(Person *, String, String);
   static void Register(Person *, String, String);
   static void Repeat(Person *, String, String);
   static void Rules(Person *, String, String);
   static void Say(Person *, String, String);
   static void Server(Person *, String, String);
   static void ServerList(Person *, String, String);
   static void Stats(Person *, String, String);
   static void Test(Person *, String, String);
   static void Time(Person *, String, String);
   static void Topic(Person *, String, String);
   static void UserList(Person *, String, String);
   static void Voice(Person *, String, String);
};

#endif
