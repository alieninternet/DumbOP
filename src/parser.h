#ifndef __PARSER_H_
# define __PARSER_H_

# include "string.h"
# include "person.h"
# include "serverconnection.h"

struct CTCPFunctionsStruct {
   const char * name;
   void (*function)(ServerConnection *, Person *, String);
   bool needsIdent;
   bool hidden;
   const char * usage;
};

class CTCPFunction {
 public:
   String name;
   void (*function)(ServerConnection *, Person *, String);
   bool needsIdent;
   bool hidden;
   String usage;
   
   CTCPFunction(String na, void (*f)(ServerConnection *, Person *, String),
		bool i, bool h, String u)
     : name(na), function(f), needsIdent(i), hidden(h), usage(u)
       {
       }
};

struct userFunctionsStruct {
   const char * name;
   void (*function)(ServerConnection *, Person *, String, String);
   int minLevel;
   bool needsChannelName;
};

class userFunction {
 public:
   String name;
   void (*function)(ServerConnection *, Person *, String, String);
   int minLevel;
   bool needsChannelName;
   
   userFunction(String na, void (*f)(ServerConnection *, Person *,
				     String, String),
		int m, bool n)
     : name(na), function(f), minLevel(m), needsChannelName(n)
     {
     }
};

class Parser {
 public:
   static void parseLine(ServerConnection *, String);
   
   static void parse001(ServerConnection *, Person *, String);
   static void parse302(ServerConnection *, Person *, String);
   static void parse303(ServerConnection *, Person *, String);
   static void parse311(ServerConnection *, Person *, String);
   static void parse315(ServerConnection *, Person *, String);
   static void parse324(ServerConnection *, Person *, String);
   static void parse332(ServerConnection *, Person *, String);
   static void parse352(ServerConnection *, Person *, String);
   static void parse353(ServerConnection *, Person *, String);
   static void parse366(ServerConnection *, Person *, String);
   static void parse367(ServerConnection *, Person *, String);
   static void parse401(ServerConnection *, Person *, String);
   static void parse433(ServerConnection *, Person *, String);
   static void parse473(ServerConnection *, Person *, String);
   static void parseError(ServerConnection *, Person *, String);
   static void parseInvite(ServerConnection *, Person *, String);
   static void parseJoin(ServerConnection *, Person *, String);
   static void parseKick(ServerConnection *, Person *, String);
   static void parseMode(ServerConnection *, Person *, String);
   static void parseNick(ServerConnection *, Person *, String);
   static void parseNotice(ServerConnection *, Person *, String); 
   static void parsePart(ServerConnection *, Person *, String);
   static void parsePing(ServerConnection *, Person *, String);
   static void parsePong(ServerConnection *, Person *, String);
   static void parsePrivmsg(ServerConnection *, Person *, String);
   static void parseQuit(ServerConnection *, Person *, String);
   static void parseTopic(ServerConnection *, Person *, String);
   
   static void parseCTCP(ServerConnection *, Person *, String,
			 String);
   static void parseMessage(ServerConnection *, Person *, String,
			    String);

   static void sendNotice(Person *, String);
};

#endif
