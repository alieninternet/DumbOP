
#include "config.h"
#include "parser.h"
#include "tables.h"
#include "utils.h"


/* parseLine - Parse a line from the server then act upon it
 * Original 12/12/00, Pickle <pickle@alien.net.au>
 */
void Parser::parseLine(ServerConnection *cnx, String line)
{
   StringTokens st(line);
   Person * from = 0;
   String fromMask = "";
   
   cnx->receivedLen += line.length();

   // Check if the line is a server statement
   if (line[0] == ':') {
      fromMask = st.nextToken().subString(1);
   }
      // If this is a mask, make up a 'person' pointer for this line
//      if (fromMask.find('!') != -1) {
	 from = new Person(fromMask, cnx,
			   cnx->bot->userList->getUserListItem(fromMask));
//      }
//   }
   
   String command = st.nextToken();
   String rest = st.rest();
      
   // Check if we have a function to parse
   for (int i = 0; userParserFunctions[i].name != 0; i++) {
      if (command == userParserFunctions[i].name) {
	 userParserFunctions[i].function(from, Utils::dwindleSpaces(rest));
	 break;
      }
   }

   // Feed this line to the monitoring and logging agents, it may be wanted
   cnx->bot->telnetDaemon->monitorLine(fromMask, command, rest);
   // logging here?
   
   delete from;
}
