
#include "parser.h"
#include "ctcp.h"
#include "bot.h"
   
// CTCPFunctionList - Table of CTCP commands
struct CTCPFunctionsStruct CTCPFunctionsInit[] =
{
     { "ACTION",	CTCP::Action,		false,	true,
	"" },
     { "CLIENTINFO",	CTCP::ClientInfo,	false,	false,
	"[<query>]" },
     { "DCC",		CTCP::DCC,		true,	false,
	"<type> [<argument>] [<address>] [<port>] [<size>]" },
     { "ECHO",		CTCP::Echo,		false,	false,
	"[<string>]" },
     { "ERRMSG",	CTCP::ErrMsg,		false,	false,
	"[<query>]" },
     { "FINGER",	CTCP::Finger,		false,	false,
	"" },
     { "LAG",		CTCP::Lag,		false,	false,
	"" },
     { "PING",		CTCP::Ping,		false,	false,
	"<time>" },
     { "SEX",		CTCP::Sex,		false,	true,
	"" },
     { "SOURCE",	CTCP::Source,		false,	false,
	"" },
     { "TIME",		CTCP::Time,		false,	false,
	"" },
     { "UPTIME",	CTCP::Uptime,		false,	false,
	"" },
     { "USERINFO",	CTCP::UserInfo,		false,	false,
	"" },
     { "VERSION",	CTCP::Version,		false,	false,
	"" },
     { "",		0,			false,	false,	"" }
   
};



/* parseCTCP - Parse a CTCP request
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 * 30/12/00 Pickle - Changed over to open function list instead of IF-swamps
 * Needs: Identification code added (non-critical)
 */
void Parser::parseCTCP(ServerConnection *cnx,
		       Person *from, String to,
		       String parameters)
{
   bool found;
   StringTokens st(parameters);
   String command = st.nextToken().toUpper();
   String rest = "";
   
   if (st.hasMoreTokens())
     rest = st.rest();
   
   for (list<CTCPFunction *>::iterator it = cnx->bot->CTCPFunctions.begin();
	it != cnx->bot->CTCPFunctions.end(); it++)
     if (command == (*it)->name) {
	found = true;
	if (!((*it)->needsIdent) ||
	    (((*it)->needsIdent) && 
	     true)) { // Should check if identified if required
	   (*it)->function(cnx, from, rest);
	} else
	  from->sendCTCPReply("ERRMSG",
			      "You are required to be identified to for this query");
	break;
     }
   
   if (!found)
     from->sendCTCPReply("ERRMSG", "Unsupported CTCP query");
}

