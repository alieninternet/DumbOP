
#include "config.h"
#include "parser.h"
#include "bot.h"
#include "serverqueue.h"
#include "flags.h"
#include "utils.h"

void Parser::parseError(ServerConnection *cnx, Person *from, String rest)
{
   cnx->bot->logLine(String("Error from server ") +
		     cnx->bot->serverList->currentServer()->getHostName() +
		     " (" + 
		     String((long)cnx->bot->serverList->currentServer()->getPort()) +
		     ").");
   cnx->bot->nextServer();
}


void Parser::parseInvite(ServerConnection *cnx, Person *from, String rest)
{
   String nick = from->getNick();
   StringTokens st(rest);
   st.nextToken(':');
   String channel = st.rest();
   
   if (cnx->bot->wantedChannels.find(channel) !=
       cnx->bot->wantedChannels.end()) {
      cnx->queue->sendJoin(channel, cnx->bot->wantedChannels[channel]->key);
   }
}


void Parser::parseJoin(ServerConnection *cnx, Person *from, String rest)
{
   StringTokens st(from->getAddress());
   String n = st.nextToken('!');
   String uh = st.nextToken();
   StringTokens st2(rest);
   String c = st2.nextToken(':');
   String mode;
   bool joinAndMode = false;
   
   // This part of code is for the combined JOIN & MODE of ircd 2.9
   if (c.find('\007') >= 0) {
      joinAndMode = true;
      StringTokens st3(c);
      c = st3.nextToken('\007');
      String m = st3.rest();
      mode = c + " +" + m;
      
      for (int i = 0; i < m.length(); i++) {
	mode = mode + " " + n;    
      }
   }
   
   if (n == cnx->bot->nickName) {
      cnx->bot->logLine(String("Joined channel ") + c + ".");
      
      if (cnx->bot->wantedChannels.find(c) != cnx->bot->wantedChannels.end()) {
	cnx->bot->channelList->addChannel(cnx, c, cnx->bot->wantedChannels[c]->keep);
      } else {
	 cnx->bot->channelList->addChannel(cnx, c);
      }
      
      cnx->queue->sendWho(c);
      cnx->queue->sendChannelMode(String("MODE ") + c + " b");
      cnx->queue->sendChannelMode(String("MODE ") + c);
   } else {
      Channel * ch = cnx->bot->channelList->getChannel(c);
      
      if (!ch) {
	 return;
      }
      
      ch->addNick(n, uh, 0, cnx->bot->userList);
      
      // Check if we have a greeting message we should send this user
      if (cnx->bot->wantedChannels[ch->channelName]->greeting.length()) {
	 from->sendNotice(cnx->bot->wantedChannels[ch->channelName]->greeting);
      }
      
      // If it is a game channel, the game engine may need this data too.
      if (cnx->bot->wantedChannels[ch->channelName]->flags & 
	  CHANFLAG_ALLOW_GAMES) {
	 cnx->bot->games->parseJoin(ch, from);
      }
   }
   
   if (joinAndMode) {
      parseMode(cnx, 0, mode);
   }
}


void Parser::parseKick(ServerConnection *cnx, Person *from, String rest)
{
   StringTokens st(rest);
   String channel = st.nextToken();
   String target = st.nextToken();
   String reason = st.rest().subString(1);
   
   if (target == cnx->bot->nickName) {
      cnx->bot->logLine(from->getAddress() + " kicked me out of channel " +
			channel + " (" + reason + ").");
      cnx->queue->sendJoin(channel, cnx->bot->channelList->getChannel(channel)->channelKey);
      cnx->bot->channelList->delChannel(channel);
   } else {
      if (!cnx->bot->channelList->getChannel(channel)) {
	 return;
      }
      
      User *u = cnx->bot->channelList->getChannel(channel)->getUser(target);
    
      if (u && u->getProt() >= User::NO_KICK) {
	 String fromNick = from->getNick();
	 User *v = cnx->bot->channelList->getChannel(channel)->getUser(fromNick);
	 if (v->getProt() < User::NO_KICK) {
	    cnx->bot->logLine(from->getAddress() + " kicked " + target +
			      " (protected) out of channel " + channel +
			      " (" + reason + ").");
	    cnx->queue->sendKick(channel, fromNick,
				 target + " \002is protected !\002");
	 }
      }

      // Delete the nickname from that channel
      cnx->bot->channelList->getChannel(channel)->delNick(target);
   }
}


void Parser::parseMode(ServerConnection *cnx, Person *from, String rest)
{
   StringTokens st(rest);
   String ch = st.nextToken();
   String modes = st.rest();
   
   if (Utils::isChannel(ch)) {
      Channel *c = cnx->bot->channelList->getChannel(ch);
      
      if (!c) {
	 return;
      }
      
      if (from) {
	 c->parseMode(from, modes);
      } else {
	 c->parseMode(0, modes);
      }
   }
}


void Parser::parseNick(ServerConnection *cnx, Person *from, String rest)
{
   String oldNick = from->getNick();
   String newNick = rest.subString(1);
   
   for (map<String, Channel *, less<String> >::iterator it = 
	cnx->bot->channelList->begin(); 
	it != cnx->bot->channelList->end(); ++it) {
      if ((*it).second->hasNick(oldNick)) {
	 (*it).second->changeNick(oldNick, newNick);
      }
   }
}


void Parser::parseNotice(ServerConnection *cnx, Person *from, String rest)
{
   String nick = "";
   
  if (from) {
     nick = from->getNick();
  }
   
   StringTokens st(rest);
   String to = st.nextToken();
   
   rest = st.rest().subString(1);
   
   if (rest[0] != '\001') {
    return;
   }
   
   rest = rest.subString(1, rest.length() - 2);
   StringTokens st2(rest);
   String command = st2.nextToken();
   rest = st2.rest();

   if (command == "PING") {
      StringTokens st3(rest);
      time_t time_secs = atol(st3.nextToken(':'));
      unsigned short time_usecs = atoi(st3.rest());
      
      /* The internals of this process can be big -- eg. long long */
      long long pingpong = ((cnx->bot->currentTime.time * 1000) + 
			    cnx->bot->currentTime.millitm) -
	((time_secs * 1000) + time_usecs);
      
      from->sendNotice(String("Ping shows a lag between us of \002") +
		       Utils::timeBigToStr((long)pingpong) +
		       String("\002") +
		       ((cnx->lag > 0) ? 
			(String(" (My connection is lagged by \002") +
			 Utils::timeBigToStr(cnx->lag) + String("\002)")) :
			(String(""))));
   }
}


void Parser::parsePrivmsg(ServerConnection *cnx, Person *from, String rest)
{
   String nick = from->getNick();
   
   StringTokens st(rest);
   String to = st.nextToken();
   String fromUserhost = Utils::getUserhost(from->getAddress());
   
   rest = st.rest().subString(1);
   
   if (++(cnx->bot->ignoredUserhosts[fromUserhost]) > Bot::MAX_MESSAGES) {
      if (cnx->bot->ignoredUserhosts[fromUserhost] == Bot::MAX_MESSAGES+1) {
	 cnx->bot->ignoredUserhosts[fromUserhost] += Bot::IGNORE_DELAY;
	 cnx->bot->logLine(from->getAddress() +
			   " is flooding me. We will ignore him/her/it.");
	 if (!Utils::isChannel(to)) {
	    from->sendNotice(String("Stop flooding me. You are now being ignored for ") +
			     String((long)Bot::IGNORE_DELAY) + " seconds.\002");
	 }
      }
      // The following lines reset the counter if you use the
      // command "!sorry" (if '!' is your command char).
      if (rest.toUpper() == String(cnx->bot->commandChar) + "SORRY") {
	 cnx->bot->ignoredUserhosts[fromUserhost] = 0;
	 from->sendNotice("\002Don't do it again!\002");
      }
      return;
   }
   
   if (rest[0] == '\001') {
      rest = rest.subString(1, rest.length() - 2);
#ifdef NOT_VERY_HAPPY_ABOUT_CHANNEL_CTCP_BROADCASTS_WORKING_NO_SIREE_BOB
      if (!Utils::isChannel(to)) 
#endif
	Parser::parseCTCP(cnx, from, to, rest);
   } else {
      Parser::parseMessage(cnx, from, to, rest);
   }
}


void Parser::parsePart(ServerConnection *cnx, Person *from, String rest)
{
   String n = from->getNick();
   StringTokens st(rest);
   String channel = st.nextToken();
   
   if (n.toLower() == cnx->bot->nickName.toLower()) {
      cnx->bot->logLine(String("Left channel ") + channel + ".");
      cnx->bot->channelList->delChannel(channel);
   } else {
      Channel * c = cnx->bot->channelList->getChannel(channel);
      
      if (!c) {
	 return;
      }
      
      c->delNick(n);
      
      if (c->countOp == 0 && c->count == 1) {
	 cnx->queue->sendPart(channel, "");
	 cnx->queue->sendJoin(channel, cnx->bot->wantedChannels[channel]->key);
      }

      // If it is a game channel, the game engine may need this data too.
      if (cnx->bot->wantedChannels[c->channelName]->flags & 
	  CHANFLAG_ALLOW_GAMES) {
	 cnx->bot->games->parseLeave(c, from);
      }
   }
}


void Parser::parsePing(ServerConnection * cnx, Person *from, String rest)
{
   cnx->queue->sendPong(rest);
}


void Parser::parsePong(ServerConnection *cnx, Person *from, String rest)
{
   cnx->lag = (((cnx->bot->currentTime.time * 1000) + 
		cnx->bot->currentTime.millitm) -
	       ((cnx->pingTime.time * 1000) +
		cnx->pingTime.millitm));
   cnx->bot->sentPing = false;
}


void Parser::parseQuit(ServerConnection *cnx, Person *from, String rest)
{
   String n = from->getNick();
   
   if (n == cnx->bot->nickName) {
      cnx->bot->stop = true;
   }
   
   for (map<String, Channel *, less<String> >::iterator it =
	cnx->bot->channelList->begin();
	it != cnx->bot->channelList->end();
	++it) {
      (*it).second->delNick(n);
   }
}


void Parser::parseTopic(ServerConnection *cnx, Person *from, String rest)
{
   StringTokens st(rest);
   String channel = st.nextToken();
   String newTopic = st.rest().subString(1);
   Channel *c = cnx->bot->channelList->getChannel(channel);
   
  if (!c) {
     return;
  }

   if (c->lockedTopic && from->getNick() != cnx->bot->nickName) {
      cnx->queue->sendTopic(channel, c->channelTopic);
   }
   
   c->channelTopic = newTopic;
}


void Parser::parseWallops(ServerConnection *cnx, Person *from, String rest)
{
   cnx->bot->logLine(String("WALLOPS from ") + from->getAddress() +
		     String(" (") + rest + String(")"));
}
