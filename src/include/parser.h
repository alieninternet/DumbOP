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
   
   static void parse001(Person *, String);
   static void parse302(Person *, String);
   static void parse303(Person *, String);
   static void parse311(Person *, String);
   static void parse315(Person *, String);
   static void parse324(Person *, String);
   static void parse332(Person *, String);
   static void parse352(Person *, String);
   static void parse353(Person *, String);
   static void parse366(Person *, String);
   static void parse367(Person *, String);
   static void parse401(Person *, String);
   static void parse433(Person *, String);
   static void parse473(Person *, String);
   static void parseError(Person *, String);
   static void parseInvite(Person *, String);
   static void parseJoin(Person *, String);
   static void parseKick(Person *, String);
   static void parseMode(Person *, String);
   static void parseNick(Person *, String);
   static void parseNotice(Person *, String); 
   static void parsePart(Person *, String);
   static void parsePing(Person *, String);
   static void parsePong(Person *, String);
   static void parsePrivmsg(Person *, String);
   static void parseQuit(Person *, String);
   static void parseTopic(Person *, String);
   static void parseWallops(Person *, String);
   
   static void parseCTCP(Person *, String, String);
   static void parseMessage(Person *, String, String);
};

#endif
