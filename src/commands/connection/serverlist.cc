
#include "config.h"
#include "commands.h"
#include "utils.h"

/* merge with server */
void Commands::ServerList(ServerConnection *cnx, Person *from,
			      String channel, String rest)
{
   String nick = from->getNick();
   
   from->sendNotice("\002Server list:\002");
   long l = 0;
   String s;
   for (vector<class Server *>::iterator it =
	cnx->bot->serverList->v.begin();
	it != cnx->bot->serverList->v.end();
	++it) {
      s = (*it) == cnx->server ? "\026" : "";
      from->sendNotice(s + String(l++) + s + " " +
		       (*it)->getHostName() +
		       " (" + String((long)((*it)->getPort())) +
		       ")");
   }
   
   from->sendNotice("\002End of server list.\002");
}

