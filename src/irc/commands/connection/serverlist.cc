
#include "config.h"
#include "commands.h"
#include "utils.h"

/* merge with server */
void Commands::ServerList( Person *from,
			      String channel, String rest)
{
   String nick = from->getNick();
   
   from->sendLine("\002Server list:\002");
   long l = 0;
   String s;
   for (vector<class Server *>::iterator it =
	from->cnx->bot->serverList->v.begin();
	it != from->cnx->bot->serverList->v.end();
	++it) {
      s = (*it) == from->cnx->server ? "\026" : "";
      from->sendLine(s + String(l++) + s + " " +
		       (*it)->getHostName() +
		       " (" + String((long)((*it)->getPort())) +
		       ")");
   }
   
   from->sendLine("\002End of server list.\002");
}

