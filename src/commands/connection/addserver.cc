
#include "config.h"
#include "commands.h"
#include "utils.h"

/* AddServer
 * Original 18/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::AddServer(ServerConnection *cnx, Person *from,
			     String channel, String rest)
{
   if (rest.length() == 0) {
      from->sendNotice("\002You must supply a server name.\002");
      return;
   }
   
   StringTokens st(rest);
   String serverName = st.nextToken();
   int port = 6667;
   
   if (st.hasMoreTokens()) {
      String temp = st.nextToken();
      port = atoi((const char *)temp);
   }
 
   // Check that the port is ok
   if ((port <= 0) && (port >= 65535)) {
      from->sendNotice("Invalid port number given.");
      return;
   }
 
   // Add the server
   cnx->bot->serverList->addServer(new class Server(serverName, port));
   
   // Tell the user what we did
   from->sendNotice(String("Server \002") + serverName + 
		    String("\002 on port \002") + String((long)port) + 
		    String("\002 has been added to the server list."));
}

