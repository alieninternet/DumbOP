
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


/* DelServer
 * Original 18/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::DelServer(ServerConnection *cnx, Person *from,
			     String channel, String rest)
{
   if (rest.length() == 0) {
      from->sendNotice("You need to supply a server number for this command.");
      return;
   }
   
   int serverNumber = atoi(rest);
   
   if ((serverNumber < 0) || (serverNumber >= cnx->bot->serverList->size())) {
      from->sendNotice("Invalid server number.");
      return;
   }
   
   cnx->bot->serverList->delServer(serverNumber);
   
   from->sendNotice(String("Deleted server ") +
		    cnx->bot->serverList->get(serverNumber)->getHostName() +
		    " (" + String((long)cnx->bot->serverList->get(serverNumber)->getPort()) +
		    ").");
}


/* Die - Make the bot quit
 * Original 13/12/00, Pickle <pickle@alien.net.au>
 * 28/12/00 Pickle - Changed default quit message to VERSION_STRING
 */
void Commands::Die(ServerConnection *cnx, Person *from,
		       String channel, String rest)
{
   // Quit IRC
   if (rest.length() == 0) {
      cnx->queue->sendQuit(from->getNick() + 
			   String(" requested me to leave [") + 
			   VERSION_STRING + String("]"));
   } else {
      cnx->queue->sendQuit(rest);
   }
   
   // Tell the bot to stop now
   cnx->bot->stop = true;
}


/* Do
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 * Needs: Remove command.c dependancy
 */
void Commands::Do(ServerConnection *cnx, Person *from,
		      String channel, String rest)
{
   if (!rest.length()) {
      from->sendNotice("And do what?");
   } else {
      cnx->queue->sendPrivmsg(channel, String("\001ACTION ") + rest +
			      String("\001"));
   }
}


/* Merge with Server */
void Commands::NextServer(ServerConnection *cnx, Person *from,
			      String channel, String rest)
{
   if (cnx->bot->serverList->size() == 0) {
      from->sendNotice("\002Server list is empty !\002");
      return;
   }
   
   if (cnx->bot->serverList->size() == 1) {
      from->sendNotice("\002Server list has only one item. Use"
		       " \"reconnect\" to force reconnection.\002");
      return;
   }
   
   if (cnx->bot->canChangeServer()) {
      cnx->queue->sendQuit("Changing server");
      cnx->bot->nextServer();
   } else {
      from->sendNotice("\002I can not change server without"
		       " losing op on a channel I am on.\002");
   }
}


/* Nick - Change nickname
 * Original 14/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::Nick(ServerConnection *cnx, Person *from,
			String channel, String rest)
{
   StringTokens st(rest);
   String nick = st.nextToken();
   
   if (rest == "") {
      from->sendNotice(String("\002No nickname given.\002"));
      return;
   }
   
   if (!Utils::isValidNickName(nick)) {
      from->sendNotice(String("\002") + nick +
		       " is not a valid nickname\002");
      return;
   }
   
   cnx->bot->wantedNickName = nick;
   cnx->queue->sendNick(nick);
}


/* Server
 * Original 14/12/00, Pickle <pickle@alien.net.au>
 * Needs: Join serverlist and nextserver and reconnect into this command
 */
void Commands::Server(ServerConnection *cnx, Person *from,
			  String channel, String rest)
{
   int serverNumber;
   String nick = from->getNick();
   
   if (rest.length() == 0) {
      from->sendNotice("\002You need to supply a server number for this command.\002");
      return;
   }
   
   serverNumber = atoi(rest);
   
   if (serverNumber < 0 || serverNumber >= cnx->bot->serverList->size()) {
      from->sendNotice(String((long)serverNumber) +
		       " \002is an invalid server number (see the serverlist).\002");
      return;
   }
   
   if (cnx->bot->canChangeServer()) {
      cnx->queue->sendQuit("Changing server");
      cnx->queue->flush();
      cnx->bot->connect(serverNumber);
   } else {
      from->sendNotice("\002I can not change server without losing op on a channel I am on.\002");
   }
}


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


/* Raw - Write a raw line to the server
 * Original - 11/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::Raw(ServerConnection *cnx, Person *from,
		       String channel, String rest)
{
   if (rest.length() != 0) {
      cnx->queue->addLine(rest, 0, 0, ServerQueueItem::OTHER);
   }
}


/* merge with server */
void Commands::Reconnect(ServerConnection *cnx, Person *from,
			     String channel, String rest)
{
   String nick = from->getNick();
   
   if (cnx->bot->canChangeServer()) {
      cnx->queue->sendQuit("Reconnecting");
      cnx->bot->reconnect();
   } else {
      from->sendNotice("\002I can not change server without losing op on a channel I am on.\002");
   }
}


/* Say - Say something to a channel
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::Say(ServerConnection *cnx, Person *from,
		       String channel, String rest)
{
   if (!rest.length()) {
      from->sendNotice("And say what?");
   } else {
      cnx->queue->sendPrivmsg(channel, rest);
   }
}

