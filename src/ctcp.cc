/* ctcp.cc
 * Parse CTCP commands
 */

#include <time.h>

#include "ctcp.h"
#include "parser.h"
#include "utils.h"
#include "stringtokenizer.h"
#include "str.h"
#include "bot.h"
#include "person.h"
#include "version.h"


/* Action - If in logging mode, log a /me for the appropriate channel
 * Notes: No reply, informative only
 */
void CTCP::Action(ServerConnection *cnx, Person *from, String rest)
{
   // Add stuff here when the channel logging code is made.. whenever that is
}


/* ClientInfo - For other hackers, to show off what CTCP commands we can do :)
 * Notes: Format - CLIENTINFO :<string command> [<string command>]...
 *        Unless the query matches a CTCP command we know, if then we must go
 *        into detail as stored within the CTCP table in parser.c
 * Needs: Finishing? Gee, that'd be a good idea for most of this codebase :)
 */
void CTCP::ClientInfo(ServerConnection *cnx, Person *from, String rest)
{
   if (rest.length() != 0) {
      StringTokenizer st(rest);
      String query = st.nextToken();
      
      for (list<CTCPFunction *>::iterator it = cnx->bot->CTCPFunctions.begin();
	   it != cnx->bot->CTCPFunctions.end(); it++)
	if ((query.toUpper() == (*it)->name) &&
	    !((*it)->hidden))
	  if ((*it)->usage != "") {
	     from->sendCTCPReply("CLIENTINFO", String((*it)->name) + 
				 String(" ") + String((*it)->usage));
	     return;
	  } else {
	     from->sendCTCPReply("ERRMSG", String(rest) + String(" :") +
				 String("No further information on CTCP command queried"));
	     return;
	  }
      from->sendCTCPReply("ERRMSG", String(rest) + String(" :") +
			  String("No information on unsupported CTCP command queried"));
   } else {
      String result = "";
      
      for (list<CTCPFunction *>::iterator it = cnx->bot->CTCPFunctions.begin();
	   it != cnx->bot->CTCPFunctions.end(); it++)
	if (!((*it)->hidden)) {
	   result = result + (*it)->name + " ";
	   if (result.length() >= 256) {
	      from->sendCTCPReply("CLIENTINFO", result);
	      result = ":";
	   }
	}
      if (result.length() > 1)
	from->sendCTCPReply("CLIENTINFO", result);
   }
}


/* DCC - Parse DCC commands
 */
void CTCP::DCC(ServerConnection *cnx, Person *from, String rest)
{
   if (rest.length() == 0) {
      from->sendCTCPReply("ERRMSG", String(rest) + String(" :") +
			  String("This query requires more information"));
      return;
   }
   
   StringTokenizer st(rest);
   String type, argument, address, port, size;
   
   type = st.nextToken().toUpper();
   argument = st.nextToken();
   address = st.nextToken();
   port = st.nextToken();
   size = st.nextToken();
   
   if (type == "CHAT") {
      from->sendCTCPReply("ERRMSG", String(rest) + String(" :") +
			  String("Incomplete command."));
   } else if (type == "TALK") {
      from->sendCTCPReply("ERRMSG", String(rest) + String(" :") +
			  String("Non-living client cannot accept TALK request"));
   } else if (type == "SEND") {
      from->sendCTCPReply("ERRMSG", String(rest) + String(" :") +
			  String("Incomplete command."));
   } else
     from->sendCTCPReply("ERRMSG", String(rest) + String(" :") +
			 String("Unsupported DCC type"));
}


/* Echo - Return whatever was sent to us
 * Original 30/12/00, Pickle <pickle@alien.net.au>
 */
void CTCP::Echo(ServerConnection *cnx, Person *from, String rest)
{
   from->sendCTCPReply("ECHO", String(rest));
}


/* ErrMsg - Parse an error message
 * Original 29/12/00, Pickle <pickle@alien.net.au>
 * Notes: Format - ERRMSG <string query> :<string errormsg>
 *        When used as a query, we are required to say there wasn't an error
 *        (kind of like telling the sender they're a bloody twat)
 */
void CTCP::ErrMsg(ServerConnection *cnx, Person *from, String rest)
{
   from->sendCTCPReply("ERRMSG", String(rest) + String(" :") +
		       String("No error, test responce OK."));
}


/* Finger - Parse a finger request (idle time)
 * Original 29/12/00, Pickle <pickle@alien.net.au>
 * Notes: Format - FINGER :<string human_readable>
 * Needs: To be useful perhaps? :)
 */
void CTCP::Finger(ServerConnection *cnx, Person *from, String rest)
{
   from->sendCTCPReply("FINGER", String(":") + VERSION_STRING);
}


/* Lag - Show current Client <-> Server lag count
 * Original 29/12/00, Pickle <pickle@alien.net.au>
 * Notes: Format - LAG <time_t lagtime>:<string human_readable>
 */
void CTCP::Lag(ServerConnection *cnx, Person *from, String rest)
{
   from->sendCTCPReply("LAG", String((long)(cnx->lag / 1000)) + String(":") +
		       Utils::timeBigToStr(cnx->lag));
}


/* Ping - Respond to a ping request, or reply
 * Original 29/12/00, Pickle <pickle@alien.net.au>
 * Notes: Format - PING <string query>
 */
void CTCP::Ping(ServerConnection *cnx, Person *from, String rest)
{
   from->sendCTCPReply("PING", String(rest));
}


/* Sex - Show what sex I apparenly am
 * Original 29/12/00, Pickle <pickle@alien.net.au>
 * Notes: This command is non-conventional
 */
void CTCP::Sex(ServerConnection *cnx, Person *from, String rest)
{
   from->sendCTCPReply("SEX", "With you? HAHAHAHA!");
}


/* Source - Show where a user can grab a copy of this software
 * Original 29/12/00, Pickle <pickle@alien.net.au>
 * Notes: Format - SOURCE <string host>:<string directory>:<string filename>
 */
void CTCP::Source(ServerConnection *cnx, Person *from, String rest)
{
   // This program aint released, so we wont give them any useful info
   from->sendCTCPReply("SOURCE", "www.alien.net.au::");
}


/* Time - Show the current local system time
 * Original 29/12/00, Pickle <pickle@alien.net.au>
 * Notes: Format - TIME <string human_readable>
 */
void CTCP::Time(ServerConnection *cnx, Person *from, String rest)
{
   String time = String(ctime(&cnx->bot->currentTime.time));
   
   from->sendCTCPReply("TIME", 
		       time.subString(0,time.length()-2));
}


/* Uptime - Show current program uptime
 * Original 29/12/00, Pickle <pickle@alien.net.au>
 * Notes: Format - UPTIME <time_t uptime>:<string human_readable>
 */
void CTCP::Uptime(ServerConnection *cnx, Person *from, String rest)
{
   time_t diff = cnx->bot->currentTime.time - cnx->bot->startTime;
   
   from->sendCTCPReply("UPTIME", String((long)diff) + String(":") + 
		       Utils::timelenToStr(diff));
}


/* Userinfo - Show user info string (generated)
 * Original 29/12/00, Pickle <pickle@alien.net.au>
 * Notes: Format - USERINFO :<string human_readable>
 */
void CTCP::UserInfo(ServerConnection *cnx, Person *from, String rest)
{
   // Really should be a nicer string I think.. 
   // DumbOP is smarter than people give him credit for.
   from->sendCTCPReply("USERINFO", 
		       "DumbOP, the stupidest bot on IRC.");
}


/* Version - Show version information
 * Original 29/12/00, Pickle <pickle@alien.net.au>
 * Notes: Format - VERSION <string client_name>:<string version>:<string env>
 */
void CTCP::Version(ServerConnection *cnx, Person *from, String rest)
{
   from->sendCTCPReply("VERSION", String(PROGNAME_STRING) + String(":") + 
		       String(VERNUM_STRING) + String(":") +
		       String(COMPILE_STRING));
}

