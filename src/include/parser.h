#ifndef __PARSER_H_
# define __PARSER_H_

# include "strings.h"
# include "person.h"
# include "serverconnection.h"

struct CTCPFunctionsStruct {
   const char * name;
   void (*function)(Person *, String);
   bool needsIdent;
   bool hidden;
   const char * usage;
};

class CTCPFunction {
 public:
   String name;
   void (*function)(Person *, String);
   bool needsIdent;
   bool hidden;
   String usage;
   
   CTCPFunction(String na, void (*f)(Person *, String),
		bool i, bool h, String u)
     : name(na), function(f), needsIdent(i), hidden(h), usage(u)
       {
       }
};

struct userFunctionsStruct {
   const char * name;
   void (*function)(Person *, String, String);
   int minLevel;
   bool needsChannelName;
};

class userFunction {
 public:
   String name;
   void (*function)(Person *, String, String);
   int minLevel;
   bool needsChannelName;
   bool hidden;
   
   userFunction(String na, void (*f)(Person *, String, String),
		int m, bool n, bool h)
     : name(na), function(f), minLevel(m), needsChannelName(n), hidden(h)
     {
     }
};

class Parser {
 public:
   static void parseLine(ServerConnection *, String);
   static void parseCTCP(Person *, String, String);
   static void parseMessage(Person *, String, String);
   
   // User type connection
   static void parseUser001(Person *, String);
   static void parseUser302(Person *, String);
   static void parseUser303(Person *, String);
   static void parseUser311(Person *, String);
   static void parseUser315(Person *, String);
   static void parseUser324(Person *, String);
   static void parseUser332(Person *, String);
   static void parseUser352(Person *, String);
   static void parseUser353(Person *, String);
   static void parseUser366(Person *, String);
   static void parseUser367(Person *, String);
   static void parseUser401(Person *, String);
   static void parseUser433(Person *, String);
   static void parseUser473(Person *, String);
   static void parseUserError(Person *, String);
   static void parseUserInvite(Person *, String);
   static void parseUserJoin(Person *, String);
   static void parseUserKick(Person *, String);
   static void parseUserMode(Person *, String);
   static void parseUserNick(Person *, String);
   static void parseUserNotice(Person *, String); 
   static void parseUserPart(Person *, String);
   static void parseUserPing(Person *, String);
   static void parseUserPong(Person *, String);
   static void parseUserPrivmsg(Person *, String);
   static void parseUserQuit(Person *, String);
   static void parseUserTopic(Person *, String);
   static void parseUserWallops(Person *, String);
};

#endif
