
#include "config.h"
#include "parser.h"
#include "bot.h"
#include "serverqueue.h"

/* parse001 - RPL_WELCOME
 * Original, Simon Butcher <simonb@alien.net.au>
 */
void Parser::parse001(Person *from, String rest)
{
   String temp = "";
   StringTokens st(rest);
   String realNick = st.nextToken();
   
   from->cnx->bot->connected = true;
   
//   from->cnx->queue->sendUserMode(from->cnx->bot->nickName, "+iw");

   /*******************************************************************/
   from->cnx->queue->sendNickopIdent(AUSTNET_PASSWORD);
   /*******************************************************************/
   
//   from->cnx->queue->sendWhois(from->cnx->bot->nickName);
   
   // Force a join on each channel we are requested to join..
   for (map<String, wantedChannel *, less<String> >::iterator it = 
	from->cnx->bot->wantedChannels.begin();
	it != from->cnx->bot->wantedChannels.end(); ++it) {
      from->cnx->queue->sendJoin((*it).first, (*it).second->key);
   }
   
   from->cnx->bot->logLine(String("Connected to server ") +
		     from->cnx->bot->serverList->currentServer()->getHostName() + 
		     " (" +  
		     String((long)from->cnx->bot->serverList->currentServer()->getPort()) +
		     ").");
}


/* parse302 - RPL_USERHOST
 * Original, Simon Butcher <simonb@alien.net.au>
 */
void Parser::parse302(Person *from, String rest)
{
   unsigned long num = from->cnx->bot->receivedUserhostID++;
   StringTokens st(rest);
   
   st.nextToken(':');
   
   if (st.rest().length()) {
      st.nextToken('=');
      String parameters = st.rest();
      parameters = parameters.subString(1);
      from->cnx->bot->userhostMap[num] = parameters;
   } else {
      from->cnx->bot->userhostMap[num] = "";
   }
}


/* parse303 - RPL_ISON
 * Original, Simon Butcher <simonb@alien.net.au>
 */
void Parser::parse303(Person *from, String rest)
{
   // Duh.
}


/* parse311 - RPL_WHOISUSER
 * Original, Simon Butcher <simonb@alien.net.au>
 */
void Parser::parse311(Person *from, String rest)
{
//  StringTokens st(rest);
//  st.nextToken();
//  String nuh = st.nextToken() + "!";
//  String uh = st.nextToken() + "@";
//  uh = uh + st.nextToken();
//  nuh = nuh + uh;
//  from->cnx->bot->userList->addUserFirst(nuh, "*", 0, 3, nuh, -1, -1, "");
//  from->cnx->bot->userHost = uh;
}


/* parse315 - RPL_ENDOFWHO
 * Original, Simon Butcher <simonb@alien.net.au>
 */
void Parser::parse315(Person *from, String rest)
{
   StringTokens st(rest);
   st.nextToken();
   String channel = st.nextToken();
   Channel *c = from->cnx->bot->channelList->getChannel(channel);
   
   if (!c) {
      return;
   }
   
   c->gotWho = true;
}


/* parse324 - RPL_CHANNELMODEIS
 * Original, Simon Butcher <simonb@alien.net.au>
 */
void Parser::parse324(Person *from, String rest)
{
   StringTokens st(rest);
   st.nextToken();
   String channel = st.nextToken();
   
   if (Channel *c = from->cnx->bot->channelList->getChannel(channel)) {
      if (c) {
	 c->parseMode(from, st.rest());
      }
   }
}


/* parse332 - RPL_TOPIC
 * Original, Simon Butcher <simonb@alien.net.au>
 */
void Parser::parse332(Person *from, String rest)
{
   StringTokens st(rest);
   st.nextToken();
   String channel = st.nextToken();
   
   if (Channel *c = from->cnx->bot->channelList->getChannel(channel)) {
    if (c) {
       c->channelTopic = st.rest().subString(1);
    }
   }
}


/* parse352 - RPL_WHOREPLY
 * Original, Simon Butcher <simonb@alien.net.au>
 */
void Parser::parse352(Person *from, String rest)
{
   StringTokens st(rest);
   st.nextToken();
   String ch = st.nextToken();
   String uh = st.nextToken() + "@";
   uh = uh + st.nextToken();
   st.nextToken();
   String n = st.nextToken();
   String m = st.nextToken();
   int mode = 0;
   
   for (int i = 0; i < m.length(); i++)
     switch (m[i]) {
      case 'H': 
	break;
      case 'G': 
	mode |= User::AWAY_MODE; 
	break;
      case '*': 
	mode |= User::IRCOP_MODE; 
	break;
      case '@': 
	mode |= User::OP_MODE; 
	break;
      case '+': 
	mode |= User::VOICE_MODE; 
	break;
    }
   
   if (Channel *c = from->cnx->bot->channelList->getChannel(ch)) {
      if (c) {
	 c->addNick(n, uh, mode, from->cnx->bot->userList);
      }
   }
}


/* parse353 - RPL_NAMESREPLY
 * Original, Simon Butcher <simonb@alien.net.au>
 */
void Parser::parse353(Person *from, String rest)
{
   int mode = 0;
   String nick;
   
   StringTokens st(rest);
   st.nextToken(); st.nextToken();
   
   Channel * c = from->cnx->bot->channelList->getChannel(st.nextToken());
   
   if (!c) {
      return;
   }
   
   StringTokens st2(st.nextToken(':'));
   
   while (st2.hasMoreTokens()) {
      nick = st2.nextToken();

      if (nick[0] == '@') {
	 mode = User::OP_MODE;
	 nick = nick.subString(1);
      } else if (nick[0] == '+') {
	 mode = User::VOICE_MODE;
	 nick = nick.subString(1);
      }
      
      c->addNick(nick, "", mode, 0, true);
   }
}


/* parse366 - RPL_ENDOFNAMES
 * Original, Simon Butcher <simonb@alien.net.au>
 */
void Parser::parse366(Person *from, String rest)
{
   StringTokens st(rest);
   st.nextToken();
   String ch = st.nextToken();

   if (Channel *c = from->cnx->bot->channelList->getChannel(ch)) {
      c->joined = true;
   }
}


/* parse367 - RPL_BANLIST
 * Original, Simon Butcher <simonb@alien.net.au>
 */
void Parser::parse367(Person *from, String rest)
{
   StringTokens st(rest);
   st.nextToken();
   String ch = st.nextToken();

   if (Channel *c = from->cnx->bot->channelList->getChannel(ch)) {
      c->addBan(st.nextToken(), -1);
   }
}


/* parse401 - ERR_NOSUCHNICK
 * Original, Simon Butcher <simonb@alien.net.au>
 */
void Parser::parse401(Person *from, String rest)
{
   StringTokens st(rest);
   st.nextToken();
   String nick = st.nextToken();
}


/* parse433 - ERR_NICKNAMEINUSE
 * Original, Simon Butcher <simonb@alien.net.au>
 */
void Parser::parse433(Person *from, String rest)
{
   if (from->cnx->bot->connected) {
      return;
   }

   if (from->cnx->bot->nickName.length() == 9) {
      int i;
      
      for (i = 0; (i < from->cnx->bot->nickName.length() && 
		   from->cnx->bot->nickName[i] == '_'); 
	   i++);
      
      if (i < from->cnx->bot->nickName.length()) {
	 from->cnx->bot->nickName = 
	   from->cnx->bot->nickName.subString(0, i-1) + "_" + 
	   from->cnx->bot->nickName.subString(i+1);
      } else {
	 from->cnx->bot->nickName = from->cnx->bot->nickName.subString(0, 4) +
	   String((long)(rand() % 10000));
      }
   } else {
      from->cnx->bot->nickName = from->cnx->bot->nickName + "_";
   }
   
   from->cnx->queue->sendNick(from->cnx->bot->nickName);
}


/* parse473 - RPL_UNAVAILRESOURCE
 * Original, Simon Butcher <simonb@alien.net.au>
 */
void Parser::parse473(Person *from, String rest)
{
   StringTokens st(rest);
   st.nextToken();
   
   from->cnx->bot->logLine(String("Unable to join channel ") + st.nextToken() + ".");
}

