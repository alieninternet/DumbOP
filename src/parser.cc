
#include <sys/types.h>
#include <netinet/in.h>
#include <time.h>

#include "stringtokenizer.h"
#include "parser.h"
#include "usercommands.h"
#include "ctcp.h"
#include "macros.h"
#include "utils.h"
#include "bot.h"
#include "telnetspy.h"

struct {
   char *name;
   void (*function)(ServerConnection *, Person *, String);
} functions[] = {
     { "001",	Parser::parse001     }, /* RPL_WELCOME - Welcome message */
   /* 002 - RPL_YOURHOST - Server/Version info */
   /* 003 - RPL_CREATED - Date server built */
   /* 004 - RPL_MYINFO - Detailed server info */
   /* 005 - RPL_BOUNCE - Use given server */
   /* 200 - RPL_TRACELINK - Trace link info */
   /* 201 - RPL_TRACECONNECTING - Try trace on... */
   /* 202 - RPL_TRACEHANDSHAKE - Trace handshake */
   /* 203 - RPL_TRACEUNKNOWN - Unknown trace */
   /* 204 - RPL_TRACEOPERATOR - Trace +O entry */
   /* 205 - RPL_TRACEUSER - Trace user entry */
   /* 206 - RPL_TRACESERVER - Trace server entry */
   /* 207 - RPL_TRACESERVICE - Trace service entry */
   /* 208 - RPL_TRACENEWTYPE - Trace other entry */
   /* 209 - RPL_TRACECLASS - Trace class entry */
   /* 210 - RPL_TRACERECONNECT - Reconnect trace */
   /* 211 - RPL_STATSLINKINFO - Link info stats */
   /* 212 - RPL_STATSCOMMANDS - Command stats */
   /* 213 - RPL_STATSCLINE - ? */
   /* 214 - RPL_STATSNLINE - ? */
   /* 215 - RPL_STATSILINE - ? */
   /* 216 - RPL_STATSKLINE - ? */
   /* 217 - RPL_STATSQLINE - ? */
   /* 218 - RPL_STATSYLINE - ? */
   /* 219 - RPL_ENDOFSTATS - End of stats */
   /* 221 - RPL_UMODEIS - User mode is... */
   /* 231 - RPL_SERVICEINFO - ? */
   /* 232 - RPL_ENDOFSERVICES - ? */
   /* 233 - RPL_SERVICE - ? */
   /* 234 - RPL_SERVLIST - Server list entry */
   /* 235 - RPL_SERVLISTEND - End of server list */
   /* 240 - RPL_STATSVLINE - ? */
   /* 241 - RPL_STATSLLINE - ? */
   /* 242 - RPL_STATSUPTIME - Uptime stats */
   /* 243 - RPL_STATSONLINE - Online stats */
   /* 244 - RPL_STATSHLINE - ? */
   /* 245 - RPL_STATSSLINE - ??? */
   /* 246 - RPL_STATSPING - ? */
   /* 247 - RPL_STATSBLINE - ? */
   /* 250 - RPL_STATSDLINE - ? */
   /* 251 - RPL_LUSERCLIENT - User connections info */
   /* 252 - RPL_LUSEROP - +O connections info */
   /* 253 - RPL_LUSERUNKNOWN - Unknown connections info */
   /* 254 - RPL_LUSERCHANNELS - Number of channels info */
   /* 255 - RPL_LUSERME - Local connections info */
   /* 256 - RPL_ADMINME - Local admin info */
   /* 257 - RPL_ADMINLOC1 - Local admin info (1) */
   /* 258 - RPL_ADMINLOC2 - Local admin info (2) */
   /* 259 - RPL_ADMINEMAIL - Local admin e-mail info */
   /* 261 - RPL_TRACELOG - Trace log file */
   /* 262 - RPL_TRACEEND - End of trace */
   /* 263 - RPL_TRYAGAIN - Try again soon */
   /* 300 - RPL_NONE - ? */
   /* 301 - RPL_AWAY - User is away */
     { "302",	Parser::parse302     }, /* RPL_USERHOST - ? */
     { "303",   Parser::parse303     }, /* RPL_ISON  - ? */
   /* 305 - RPL_UNAWAY - Away turned off msg */
   /* 306 - RPL_NOWAWAY - Marked as AWAY */
     { "311",	Parser::parse311     }, /* RPL_WHOISUSER - User details */
   /* 312 - RPL_WHOISSERVER - User is using this */
   /* 313 - RPL_WHOISOPERATOR - User is IRC OP */
   /* 314 - RPL_WHOWASUSER - WHOWAS details */
     { "315",	Parser::parse315     }, /* RPL_ENDOFWHO - End of WHO */
   /* 316 - RPL_WHOISCHANOP - ? */
   /* 317 - RPL_WHOISIDLE - User idle stats */
   /* 318 - RPL_ENDOFWHOIS - End of WHOIS list */
   /* 319 - RPL_WHOISCHANNELS - User is on these */
   /* 321 - RPL_LISTSTART - Start of channel LIST */
   /* 322 - RPL_LIST - Channel LIST entry */
   /* 323 - RPL_LISTEND - End of channel LIST */
     { "324",   Parser::parse324     }, /* RPL_CHANNELMODEIS - Channel mode is...*/
   /* 325 - RPL_UNIQOPIS - ?? */
   /* 331 - RPL_NOTOPIC - No topic is set */
     { "332",   Parser::parse332     }, /* RPL_TOPIC - Topic is now... */
   /* 341 - RPL_INVITING - INVITEd someone OK */
   /* 342 - RPL_SUMMONING - SUMMONing someone */
   /* 346 - RPL_INVITELIST - INVITE list entry */
   /* 347 - RPL_ENDOFINVITELIST - End of INVITE list */
   /* 348 - RPL_EXCEPTLIST - EXCEPTION list entry */
   /* 349 - RPL_ENDOFEXCEPTLIST - End of EXCEPTION list */
   /* 351 - RPL_VERSION - Server version details */
     { "352",   Parser::parse352     }, /* RPL_WHOREPLY - WHO entry */
     { "353",   Parser::parse353     }, /* RPL_NAMESREPLY - NAMES entry */
   /* 361 - RPL_KILLDONE - ? */
   /* 362 - RPL_CLOSING - ? */
   /* 363 - RPL_CLOSEEND - ? */
   /* 364 - RPL_LINKS - LINKS entry */
   /* 365 - RPL_ENDOFLINKS - End of LINKS list */
     { "366",   Parser::parse366     }, /* RPL_ENDOFNAMES - End of NAMES list */
     { "367",   Parser::parse367     }, /* RPL_BANLIST - BANLIST entry */
   /* 368 - RPL_ENDOFBANDLIST - End of BANLIST */
   /* 369 - RPL_ENDOFWHOWAS - End of WHOWAS */
   /* 371 - RPL_INFO - INFO entry */
   /* 372 - RPL_MOTD - MOTD entry */
   /* 373 - RPL_INFOSTART - ? */
   /* 374 - RPL_ENDOFINFO - End of INFO list */
   /* 375 - RPL_MOTDSTART - Start of MOTD list */
   /* 376 - RPL_ENDOFMOTD - End of MOTD list */
   /* 381 - RPL_YOUREOPER - You got +O */
   /* 382 - RPL_REHASHING - Rehashing */
   /* 383 - RPL_YOURESERVICE - You are service.. */
   /* 384 - RPL_MYPORTIS - ? */
   /* 391 - RPL_TIME - Server time */
   /* 392 - RPL_USERSSTART - Users list */
   /* 393 - RPL_USERS - Users list entry */
   /* 394 - RPL_ENDOFUSERS - End of user list */
   /* 395 - RPL_NOUSERS - Nobody logged in */
     { "401",   Parser::parse401     }, /* ERR_NOSUCHNICK - No such nick */
   /* 402 - ERR_NOSUCHSERVER - No such server */
   /* 403 - ERR_NOSUCHCHANNEL - No such channel */
   /* 404 - ERR_CANNOTSENDTOCHAN - Cannot send to channel */
   /* 405 - ERR_TOOMANYCHANNELS - On too many channels */
   /* 406 - ERR_WASNOSUCHNICK - No nick in WHOWAS history */
   /* 407 - ERR_TOOMANYTARGETS - Target list overloaded */
   /* 408 - ERR_NOSUCHSERVICE - No such service */
   /* 409 - ERR_NOORIGIN - No origin given */
   /* 411 - ERR_NORECIPIENT - No recipient given */
   /* 412 - ERR_NOTEXTTOSEND - No text given */
   /* 413 - ERR_NOTOPLEVEL - No top level domain give */
   /* 414 - ERR_WILDTOPLEVEL - Top level was wildcard */
   /* 415 - ERR_BADMASK - Invalid mask given */
   /* 421 - ERR_UNKNOWNCOMMAND - Unknown command */
   /* 422 - ERR_NOMOTD - No MOTD on server */
   /* 423 - ERR_NOADMININFO - No admin info */
   /* 424 - ERR_FILEERROR - File error */
   /* 431 - ERR_NONICKNAMEGIVEN - No nick given */
   /* 432 - ERR_ERRONEUSNICKNAME - Bad nick given */
     { "433",   Parser::parse433     }, /* ERR_NICKNAMEINUSE - Nick in use */
   /* 436 - ERR_NICKCOLLISION - Nick collision KILL */
     { "437",   Parser::parse433     }, /* ERR_UNAVAILRESOURCE - Delay block */
   /* 441 - ERR_USERNOTINCHANNEL - User not in channel */
   /* 442 - ERR_NOTONCHANNEL - Not in channel */
   /* 443 - ERR_USERONCHANNEL - Already on channel */
   /* 444 - ERR_NOLOGIN - Not logged on */
   /* 445 - ERR_SUMMONDISABLED - No SUMMON command */
   /* 446 - ERR_USERSDISABED - No USERS command */
   /* 451 - ERR_NOTREGISTERS - Not registered */
   /* 461 - ERR_NEEDMOREPARAMS - Not enough params */
   /* 462 - ERR_ALREADYREGISTERD - Already registered */
   /* 463 - ERR_NOPERMFORHOST - Host not privileged */
   /* 464 - ERR_PASSWDMISMATCH - Bad password */
   /* 465 - ERR_YOUREBANNEDCREEP - Banned from server */
   /* 466 - ERR_YOUWILLBEBANNED - Access to be denied soon */
   /* 467 - ERR_KEYSET - Channel key already set */
     { "471",   Parser::parse473     }, /* ERR_CHANNELISFULL - +l overflow */
   /* 472 - ERR_UNKNOWNMODE - Unknown mode char */
     { "473",   Parser::parse473     }, /* ERR_INVITEONLYCHAN - +i channel */
     { "474",   Parser::parse473     }, /* ERR_BANNEDFROMCHAN - +b channel */
     { "475",   Parser::parse473     }, /* ERR_BADCHANNELKEY - +k channel */
   /* 476 - ERR_BADCHANMASK - Channel mask bad */
   /* 477 - ERR_NOCHANMODES - Channel has no modes */
   /* 478 - ERR_BANLISTFULL - Too many banned masks */
   /* 481 - ERR_NOPRIVILEGES - Not a +O */
   /* 482 - ERR_CHANOPRIVSNEEDED - Not a +o */
   /* 483 - ERR_CANTKILLSERVER - Cannot kill a server */
   /* 484 - ERR_RESTRICTED - Connection restricted */
   /* 485 - ERR_UNIQOPPRIVSNEEDED - Not original +o */
   /* 491 - ERR_NOOPERHOST - No O-line for host */
   /* 492 - ERR_NOSERVICEHOST - ? */
   /* 501 - ERR_UMODEUNKNOWNFLAG - Unknown user mode char */
   /* 502 - ERR_USERSDONTMATCH - No mode change for other users */
     { "ERROR", 	Parser::parseError	},
     { "INVITE",	Parser::parseInvite	},
     { "JOIN",  	Parser::parseJoin	},
     { "KICK",  	Parser::parseKick	},
     { "MODE",  	Parser::parseMode	},
     { "NICK",		Parser::parseNick	},
     { "NOTICE",	Parser::parseNotice	},
     { "PART",  	Parser::parsePart	},
     { "PING",  	Parser::parsePing	},
     { "PONG",		Parser::parsePong	},
     { "PRIVMSG",	Parser::parsePrivmsg	},
     { "QUIT",		Parser::parseQuit	},
     { "TOPIC", 	Parser::parseTopic	},
//     { "WALLOPS",	Parser::parseWallops	},
     { "",	    	Parser::parseError	},
     { 0,         0                    }
};

/* parseLine - Parse a line from the server then act upon it
 * Original 12/12/00, Pickle <pickle@alien.net.au>
 */
void Parser::parseLine(ServerConnection * cnx, String line)
{
   StringTokenizer st(line);
   Person * from = 0;
   String fromMask = "";
   
   cnx->receivedLen += line.length();

   if (line[0] == ':') {
      fromMask = st.nextToken().subString(1);
      if (fromMask.find('!') != -1)
	from = new Person(cnx->bot, fromMask);
   }
   
   String command = st.nextToken();
   String rest = st.rest();
      
   for (int i = 0; functions[i].name != 0; i++) {
      if (command == functions[i].name) {
	 functions[i].function(cnx, from, rest);
	 break;
      }
   }

   TelnetSpy::spyLine(cnx->bot, fromMask, command, rest);
   
   delete from;
}

void
Parser::parse001(ServerConnection * cnx,
                 Person *from, String rest)
{
   String temp = "";
   StringTokenizer st(rest);
   String realNick = st.nextToken();

   cnx->bot->connected = true;

   cnx->queue->sendUserMode(cnx->bot->nickName, "+iw");

   /////////////////////////////////////////////////////////////////////
   cnx->queue->sendNickopIdent(AUSTNET_PASSWORD),
   /////////////////////////////////////////////////////////////////////

   cnx->queue->sendWhois(cnx->bot->nickName);

  for (map<String, wantedChannel *, less<String> >::iterator
         it = cnx->bot->wantedChannels.begin();
       it != cnx->bot->wantedChannels.end(); ++it)
    cnx->queue->sendJoin((*it).first, (*it).second->key);

  cnx->bot->logLine(String("Connected to server ") +
               cnx->bot->serverList->currentServer()->getHostName() +
               " (" +  String((long)cnx->bot->serverList->currentServer()->getPort()) +
               ").");
}

void
Parser::parse302(ServerConnection *cnx,
                 Person *from, String rest)
{
  unsigned long num = cnx->bot->receivedUserhostID++;
  StringTokenizer st(rest);

  st.nextToken(':');

  if (st.rest().length()) {
    st.nextToken('=');
    String parameters = st.rest();
    parameters = parameters.subString(1);
    cnx->bot->userhostMap[num] = parameters;
  } else
    cnx->bot->userhostMap[num] = "";
}

void Parser::parse303(ServerConnection *cnx, Person *from, String rest)
{
   
}

void
Parser::parse311(ServerConnection *cnx,
                 Person *from, String rest)
{
  StringTokenizer st(rest);
  st.nextToken();
  String nuh = st.nextToken() + "!";
  String uh = st.nextToken() + "@";
  uh = uh + st.nextToken();
  nuh = nuh + uh;
  cnx->bot->userList->addUserFirst(nuh, "*", 0, 3, nuh, -1, -1, "");
  cnx->bot->userHost = uh;
}

void
Parser::parse315(ServerConnection *cnx,
                 Person *from, String rest)
{
  StringTokenizer st(rest);
  st.nextToken();
  String channel = st.nextToken();
  Channel *c = cnx->bot->channelList->getChannel(channel);
  if (!c)
    return;
  c->gotWho = true;
}

void
Parser::parse324(ServerConnection *cnx,
                 Person *from, String rest)
{
  StringTokenizer st(rest);
  st.nextToken();
  String channel = st.nextToken();
  if (Channel *c = cnx->bot->channelList->getChannel(channel))
    if (c) c->parseMode(from, st.rest());
}

void
Parser::parse332(ServerConnection *cnx,
                 Person *from, String rest)
{
  StringTokenizer st(rest);
  st.nextToken();
  String channel = st.nextToken();
  if (Channel *c = cnx->bot->channelList->getChannel(channel))
    if (c) c->channelTopic = st.rest().subString(1);
}

void
Parser::parse352(ServerConnection *cnx,
                 Person *from, String rest)
{
  StringTokenizer st(rest);
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
    case 'H': break;
    case 'G': mode |= User::AWAY_MODE; break;
    case '*': mode |= User::IRCOP_MODE; break;
    case '@': mode |= User::OP_MODE; break;
    case '+': mode |= User::VOICE_MODE; break;
    }
  if (Channel *c = cnx->bot->channelList->getChannel(ch))
    if (c) c->addNick(n, uh, mode, cnx->bot->userList);
}

void
Parser::parse353(ServerConnection *cnx,
                 Person *from, String rest)
{
  int mode = 0;
  String nick;

  StringTokenizer st(rest);
  st.nextToken(); st.nextToken();

  Channel * c = cnx->bot->channelList->getChannel(st.nextToken());
  if (!c) return;
  StringTokenizer st2(st.nextToken(':'));

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

void
Parser::parse366(ServerConnection *cnx,
                 Person *from, String rest)
{
  StringTokenizer st(rest);
  st.nextToken();
  String ch = st.nextToken();
  if (Channel *c = cnx->bot->channelList->getChannel(ch))
    c->joined = true;
}

void
Parser::parse367(ServerConnection *cnx,
                 Person *from, String rest)
{
  StringTokenizer st(rest);
  st.nextToken();
  String ch = st.nextToken();
  if (Channel *c = cnx->bot->channelList->getChannel(ch))
    c->addBan(st.nextToken(), -1);
}

void
Parser::parse401(ServerConnection *cnx,
                 Person *from, String rest)
{
  StringTokenizer st(rest);
  st.nextToken();
  String nick = st.nextToken();
}

void
Parser::parse433(ServerConnection *cnx,
                 Person *from, String rest)
{
  if (cnx->bot->connected)
    return;

  if (cnx->bot->nickName.length() == 9) {
    int i;
    for (i = 0; i < cnx->bot->nickName.length() && cnx->bot->nickName[i] == '_'; i++)
       ;
     if (i < cnx->bot->nickName.length())
       cnx->bot->nickName = cnx->bot->nickName.subString(0, i-1) + "_" + cnx->bot->nickName.subString(i+1);
     else
       cnx->bot->nickName = cnx->bot->nickName.subString(0, 4) +
         String((long)(rand() % 10000));
   }
   else
     cnx->bot->nickName = cnx->bot->nickName + "_";

  cnx->queue->sendNick(cnx->bot->nickName);
}

void
Parser::parse473(ServerConnection *cnx,
                 Person *from, String rest)
{
  StringTokenizer st(rest);
  st.nextToken();

  cnx->bot->logLine(String("Unable to join channel ") +
               st.nextToken() + ".");
}

void
Parser::parseError(ServerConnection *cnx,
                   Person *from, String rest)
{
  cnx->bot->logLine(String("Error from server ") +
               cnx->bot->serverList->currentServer()->getHostName() +
               " (" + String((long)cnx->bot->serverList->currentServer()->getPort()) +
               ").");
  cnx->bot->nextServer();
}

void
Parser::parseInvite(ServerConnection *cnx,
                    Person *from, String rest)
{
  String nick = from->getNick();
  StringTokenizer st(rest);
  st.nextToken(':');
  String channel = st.rest();

  if (cnx->bot->wantedChannels.find(channel) !=
      cnx->bot->wantedChannels.end())
    cnx->queue->sendJoin(channel, cnx->bot->wantedChannels[channel]->key);
}

void
Parser::parseJoin(ServerConnection *cnx,
                  Person *from, String rest)
{
  StringTokenizer st(from->getAddress());
  String n = st.nextToken('!');
  String uh = st.nextToken();
  StringTokenizer st2(rest);
  String c = st2.nextToken(':');
  String mode;
  bool joinAndMode = false;

  // This part of code is for the combined JOIN & MODE of ircd 2.9
  if (c.find('\007') >= 0) {
    joinAndMode = true;
    StringTokenizer st3(c);
    c = st3.nextToken('\007');
    String m = st3.rest();
    mode = c + " +" + m;
    for (int i = 0; i < m.length(); i++)
      mode = mode + " " + n;    
  }

  if (n == cnx->bot->nickName) {
    cnx->bot->logLine(String("Joined channel ") + c + ".");
    if (cnx->bot->wantedChannels.find(c) != cnx->bot->wantedChannels.end())
      cnx->bot->channelList->addChannel(cnx, c, cnx->bot->wantedChannels[c]->keep);
    else
      cnx->bot->channelList->addChannel(cnx, c);
    cnx->queue->sendWho(c);
    cnx->queue->sendChannelMode(String("MODE ") + c + " b");
    cnx->queue->sendChannelMode(String("MODE ") + c);
  } else {
    Channel * ch = cnx->bot->channelList->getChannel(c);
    if (!ch)
      return;
    ch->addNick(n, uh, 0, cnx->bot->userList);
    if (ch->getUser(n)->getAop() && !(ch->getUser(n)->mode & User::OP_MODE) && cnx->bot->iAmOp(c)) {
      // This is a part of the antispoof code
      ch->getUser(n)->userkey = Utils::getKey();
      cnx->queue->sendCTCP(n, "PING", ch->getUser(n)->userkey + " " + c);
    }
  }

  if (joinAndMode)
    parseMode(cnx, 0, mode);
}

void
Parser::parseKick(ServerConnection *cnx,
                  Person *from, String rest)
{
  StringTokenizer st(rest);
  String channel = st.nextToken();
  String target = st.nextToken();
  String reason = st.rest().subString(1);

  if (target == cnx->bot->nickName) {
    cnx->bot->logLine(from->getAddress() + " kicked me out of channel " +
                 channel + " (" + reason + ").");
    cnx->queue->sendJoin(channel, cnx->bot->channelList->getChannel(channel)->channelKey);
    cnx->bot->channelList->delChannel(channel);
  } else {
    if (!cnx->bot->channelList->getChannel(channel)) return;
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
    cnx->bot->channelList->getChannel(channel)->delNick(target);
  }
}

void
Parser::parseMode(ServerConnection *cnx,
                  Person *from, String rest)
{
   StringTokenizer st(rest);
   String ch = st.nextToken();
   String modes = st.rest();

   if (Utils::isChannel(ch)) {
      Channel *c = cnx->bot->channelList->getChannel(ch);
      if (!c)
	return;
      if (from)
	c->parseMode(from, modes);
      else
	c->parseMode(0, modes);
   }
}

void
Parser::parseNick(ServerConnection *cnx,
                  Person *from, String rest)
{
  String on_orig = from->getNick();
  String on = on_orig.toLower();
  String nn = rest.subString(1);
  String nn_lower = nn.toLower();

   for (map<String, Channel *, less<String> >::iterator it = 
	cnx->bot->channelList->begin();
	it != cnx->bot->channelList->end(); ++it)
     if ((*it).second->hasNick(on))
       (*it).second->changeNick(on, nn_lower);
}

void
Parser::parseNotice(ServerConnection *cnx,
                    Person *from, String rest)
{
  String nick = "";

  if (from)
    nick = from->getNick();

  StringTokenizer st(rest);
  String to = st.nextToken();

  rest = st.rest().subString(1);

  if (rest[0] != '\001') {
    return;
  }

  rest = rest.subString(1, rest.length() - 2);
  StringTokenizer st2(rest);
  String command = st2.nextToken();
  rest = st2.rest();

/*  if (command == "PING") {
 *    StringTokenizer st3(rest);
 *    rest = st3.nextToken();
 *    String c = st3.rest();
 *    if (cnx->bot->channelList->getChannel(c) &&
 *	 cnx->bot->channelList->getChannel(c)->getUser(nick) &&
 *	 cnx->bot->channelList->getChannel(c)->getUser(nick)->getAop() &&
 *	 !(cnx->bot->channelList->getChannel(c)->getUser(nick)->mode & User::OP_MODE)
 *	 && cnx->bot->channelList->getChannel(c)->getUser(nick)->userkey == rest)
 *      cnx->queue->sendChannelMode(c, "+o", nick);
 * } */
   if (command == "PING") {
      StringTokenizer st3(rest);
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

void
Parser::parsePrivmsg(ServerConnection *cnx,
 		    Person *from, String rest)
{
   String nick = from->getNick();
   
   StringTokenizer st(rest);
   String to = st.nextToken();
   String fromUserhost = Utils::getUserhost(from->getAddress());
   
   rest = st.rest().subString(1);
   
   if (++(cnx->bot->ignoredUserhosts[fromUserhost]) > Bot::MAX_MESSAGES) {
      if (cnx->bot->ignoredUserhosts[fromUserhost] == Bot::MAX_MESSAGES+1) {
	 cnx->bot->ignoredUserhosts[fromUserhost] += Bot::IGNORE_DELAY;
	 cnx->bot->logLine(from->getAddress() +
			   " is flooding me. We will ignore him/her/it.");
	 if (!Utils::isChannel(to))
	   from->sendNotice(String("Stop flooding me. You are now being ignored for ") +
			    String((long)Bot::IGNORE_DELAY) + " seconds.\002");
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
/*      if (!Utils::isChannel(to)) */
      Parser::parseCTCP(cnx, from, to, rest);
   } else {
      Parser::parseMessage(cnx, from, to, rest);
   }
}

void Parser::parsePart(ServerConnection *cnx, Person *from, String rest)
{
   String n = from->getNick();
   StringTokenizer st(rest);
   String channel = st.nextToken();
   
   if (n.toLower() == cnx->bot->nickName.toLower()) {
      cnx->bot->logLine(String("Left channel ") + channel + ".");
      cnx->bot->channelList->delChannel(channel);
   } else {
      Channel * c = cnx->bot->channelList->getChannel(channel);
      if (!c) return;
      c->delNick(n);
      if (c->countOp == 0 && c->count == 1) {
	 cnx->queue->sendPart(channel, "");
	 cnx->queue->sendJoin(channel, cnx->bot->wantedChannels[channel]->key);
      }
   }
}

void
Parser::parsePing(ServerConnection * cnx,
                  Person *from, String rest)
{
  cnx->queue->sendPong(rest);
}

void
Parser::parsePong(ServerConnection *cnx,
                  Person *from, String rest)
{
   cnx->lag = (((cnx->bot->currentTime.time * 1000) + 
		cnx->bot->currentTime.millitm) -
	       ((cnx->pingTime.time * 1000) +
		cnx->pingTime.millitm));
   cnx->bot->sentPing = false;
}

void
Parser::parseQuit(ServerConnection *cnx,
                  Person *from, String rest)
{
  String n = from->getNick();
  

  if (n == cnx->bot->nickName)
    cnx->bot->stop = true;

  for (map<String, Channel *, less<String> >::iterator it =
         cnx->bot->channelList->begin();
       it != cnx->bot->channelList->end();
       ++it)
    (*it).second->delNick(n);
}

void
Parser::parseTopic(ServerConnection *cnx,
                   Person *from, String rest)
{
  StringTokenizer st(rest);
  String channel = st.nextToken();
  String newTopic = st.rest().subString(1);
  Channel *c = cnx->bot->channelList->getChannel(channel);

  if (!c) return;

  if (c->lockedTopic && from->getNick() != cnx->bot->nickName)
    cnx->queue->sendTopic(channel, c->channelTopic);

  c->channelTopic = newTopic;
}

   
void Parser::parseWallops(ServerConnection *cnx, Person *from, String rest)
{
   cnx->bot->logLine(String("WALLOPS from ") + from->getAddress() +
		     String(" (") + rest + String(")"));
}


   
   
   
/*
 * 
 * anything below here should go to another file?
 * 
 * 
 * 
 */
   
   
   
   
   
   
   
   
// CTCPFunctionList - Table of CTCP commands
struct CTCPFunctionsStruct CTCPFunctionsInit[] =
{
     { "ACTION",	CTCP::Action,		false,	true,
	"" },
     { "CLIENTINFO",	CTCP::ClientInfo,	false,	false,
	"[<string query>]" },
     { "DCC",		CTCP::DCC,		false,	false,
	"<string type> [<string argument>] [<int address>] [<int port>] [<int size>]" },
     { "ECHO",		CTCP::Echo,		false,	false,
	"[<string echo>]" },
     { "ERRMSG",	CTCP::ErrMsg,		false,	false,
	"[<string query>]" },
     { "FINGER",	CTCP::Finger,		false,	false,
	"" },
     { "LAG",		CTCP::Lag,		false,	false,
	"" },
     { "PING",		CTCP::Ping,		false,	false,
	"<time_t time>" },
     { "SEX",		CTCP::Sex,		false,	false,
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
   StringTokenizer st(parameters);
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

// userFunctionsInit - Table of user functions
struct userFunctionsStruct userFunctionsInit[] = {
     { "ACCESS",	UserCommands::Access,      
	User::USER,		true 
     },
     { "ADDUSER",     	UserCommands::AddUser,     
	User::MASTER,		false 
     },
     { "ADDSERVER",   	UserCommands::AddServer,   
	User::MASTER,      	false 
     },
     { "ALIAS",      	UserCommands::Alias,       
	User::MASTER,       	false 
     },
   //     { "BAN",         	UserCommands::Ban,         User::TRUSTED_USER, true, false  },
     { "BANLIST",     	UserCommands::BanList,     
	User::MASTER,        	true 
     },
     { "CATEGORY",	UserCommands::Category,
	User::MANAGER,		true
     },
     //     { "CHANGELEVEL", UserCommands::ChangeLevel, User::MASTER,       false, false },
     { "CYCLE",       	UserCommands::Cycle,       
	User::MASTER,       	true
     },
     { "DCCLIST",     	UserCommands::DCCList,     
	User::MASTER,       	false 
     },
   //     { "DEBAN",       	UserCommands::Deban,       User::TRUSTED_USER, true, false  },
     { "DELSERVER",   	UserCommands::DelServer,   
	User::MASTER,       	false 
     },
     { "DELUSER",     	UserCommands::DelUser,     
	User::MASTER,       	false 
     },
     { "DEOP",        	UserCommands::Deop,        
	User::MASTER, 	true 
     },
     { "DIE",         	UserCommands::Die,         
	User::MASTER,       	false 
     },
     { "DO",		UserCommands::Do,	   
	User::MANAGER,	       	true  
     },
     { "HELP",        	UserCommands::Help,        
	User::NONE,         	false 
     },
     { "HINT",        	UserCommands::Hint,        
	User::MANAGER,         	false 
     },
   //     { "IDENT",       	UserCommands::Ident,       User::NONE,         true, false  },
     { "INVITE",      	UserCommands::Invite,      
	User::TRUSTED_USER, 	true  
     },
     { "JOIN",        	UserCommands::Join,        
	User::MASTER,       	false 
     },
   //     { "KEEP",        	UserCommands::Keep,        User::MANAGER,       true, false  },
     { "KICK",        	UserCommands::Kick,        
	User::TRUSTED_USER, 	true  
     },
   //     { "KICKBAN",     	UserCommands::KickBan,     User::TRUSTED_USER, true, false  },
     { "LASTSEEN",	UserCommands::LastSeen,	   
	User::USER,	       	true  
     },
     { "MODE",        	UserCommands::Mode,        
	User::TRUSTED_USER, 	true  
     },
     { "NAMES",       	UserCommands::Names,       
	User::MASTER,        	true
     },
     { "NEXTSERVER",  	UserCommands::NextServer,  
	User::MANAGER,       	false 
     },
   //     { "NICK",        	UserCommands::Nick,        User::MANAGER,       false, false },
     { "NOTE",		UserCommands::Note,	   
	User::MASTER,		false
     },
     { "NSLOOKUP",    	UserCommands::NsLookup,    
	User::MASTER,        	false 
     },
     { "OP",          	UserCommands::Op,          
	User::TRUSTED_USER, 	true  
     },
     { "PART",        	UserCommands::Part,        
	User::MASTER,       	true  
     },
//     { "PASSWORD",    	UserCommands::Password,    User::USER,         true, false  },
     { "PING",        	UserCommands::Ping,
	User::USER,       	false
     },
     { "RAW",		UserCommands::Raw,	   
	User::MASTER,       	false 
     },
     { "RECONNECT",   	UserCommands::Reconnect,   
	User::MANAGER,       	false 
     },
     { "REPEAT",   	UserCommands::Repeat,
	User::MANAGER,       	false
     },
     { "SAVE",        	UserCommands::Save,        
	User::MANAGER,       	false 
     },
     { "SAY",         	UserCommands::Say,         
	User::MANAGER,        	true 
     },
     { "SERVER",      	UserCommands::Server,      
	User::MANAGER,       	false 
     },
     { "SERVERLIST",  	UserCommands::ServerList,  
	User::MANAGER,       	false 
     },
     { "STATS",		UserCommands::Stats,	   
	User::MASTER,	       	false 
     },
     { "TEST",		UserCommands::Test,	   
	User::MASTER,       	true  
     },
     { "TIME",       	UserCommands::Time,       
	User::USER, 		false
     },
     { "TOPIC",       	UserCommands::Topic,       
	User::TRUSTED_USER, 	true 
     },
     { "USERLIST",	UserCommands::UserList,	   
	User::MANAGER,       	false 
     },
     { "VOICE",		UserCommands::Voice,
	User::MASTER,  	false 
     },
     { "",	0,	0,	false }
};

   
/* parseMessage - Parse a message, checking access levels naturally
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 */
void Parser::parseMessage(ServerConnection *cnx, Person *from, String to,
			  String parameters)
{
   if (parameters[0] != cnx->bot->commandChar) {
      // Stuff goes here.
   } else {
      StringTokenizer st(parameters);
      
      String command = st.nextToken().subString(1).toUpper();
      String rest = st.rest().trim();
      int level;
      bool identified = false;
      
      list<userFunction *>::iterator it;
      for (it = cnx->bot->userFunctions.begin();
	   it != cnx->bot->userFunctions.end();
	   ++it) {
	 if (command == (*it)->name) {
	    if ((*it)->needsChannelName) {
	       if (Utils::isChannel(rest)) {
		  StringTokenizer st2(rest);
		  to = st.nextToken();
		  rest = st.rest();
	       }
	       if (!Utils::isChannel(to)) {
		  from->sendNotice("I don't know which channel you're referring to.");
		  return;
	       }
	       if (!cnx->bot->channelList->getChannel(to)) {
		  from->sendNotice(String("I am not on \002") + to +
				   String("\002"));
		  return;
	       }
	       level = Utils::getLevel(cnx->bot, from->getAddress(), to);
	       User * u = 0;
	       if (Channel *c = cnx->bot->channelList->getChannel(to))
		 u = c->getUser(from->getNick());
	       if (!u || !u->userListItem)
		 identified = true;
	       else
		 identified = u->userListItem->passwd == "" || u->userListItem->identified > 0;
	    } else {
	       level = Utils::getLevel(cnx->bot, from->getAddress());
	       identified = true;
	    }
	    if (level >= (*it)->minLevel) {
	       cnx->bot->logLine(from->getAddress() + " did " + command +
				 " " + rest);
	       (*it)->function(cnx, from, to, rest);
	       break;
	    } else {
	       if (!identified)
		 from->sendNotice(String("You are not identified on channel \002") + 
				  to + "\002 - Use \037!IDENT\037");
	    }
	 }
      }
   }
}
   
