
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
     { "001",	Parser::parse001     }, /* RPL_WELCOME */
     { "302",	Parser::parse302     }, /* RPL_USERHOST */
     { "303",   Parser::parse303     }, /* RPL_ISON */
     { "311",	Parser::parse311     }, /* RPL_WHOISUSER */
     { "315",	Parser::parse315     }, /* RPL_ENDOFWHO */
     { "324",   Parser::parse324     }, /* RPL_CHANNELMODEIS */
     { "332",   Parser::parse332     }, /* RPL_TOPIC */
     { "352",   Parser::parse352     }, /* RPL_WHOREPLY */
     { "353",   Parser::parse353     }, /* RPL_NAMESREPLY */
     { "366",   Parser::parse366     }, /* RPL_ENDOFNAMES */
     { "367",   Parser::parse367     }, /* RPL_BANLIST */
     { "401",   Parser::parse401     }, /* ERR_NOSUCHNICK */
     { "433",   Parser::parse433     }, /* ERR_NICKNAMEINUSE */
     { "437",   Parser::parse433     }, /* ERR_UNAVAILRESOURCE */
     { "471",   Parser::parse473     }, /* ERR_CHANNELISFULL */
     { "473",   Parser::parse473     }, /* ERR_INVITEONLYCHAN */
     { "474",   Parser::parse473     }, /* ERR_BANNEDFROMCHAN */
     { "475",   Parser::parse473     }, /* ERR_BADCHANNELKEY */
     { "ERROR", Parser::parseError   },
     { "INVITE",Parser::parseInvite  },
     { "JOIN",  Parser::parseJoin    },
     { "KICK",  Parser::parseKick    },
     { "MODE",  Parser::parseMode    },
     { "NICK",  Parser::parseNick    },
     { "NOTICE",Parser::parseNotice  },
     { "PART",  Parser::parsePart    },
     { "PING",  Parser::parsePing    },
     { "PONG",  Parser::parsePong    },
     { "PRIVMSG",Parser::parsePrivmsg },
     { "QUIT",  Parser::parseQuit    },
     { "TOPIC", Parser::parseTopic   },
     { "",      Parser::parseError   },
     { 0,         0                    }
};

/* parseLine - Parse a line from the server then act upon it
 * Original 12/12/00, Pickle <pickle@alien.net.au>
 */
void Parser::parseLine(ServerConnection * cnx, String line)
{
   StringTokenizer st(line);
   Person * from = 0;
   
   cnx->bot->receivedLen += line.length();
   
   if (line[0] == ':') {
      String fromMask = st.nextToken().subString(1);
      if (fromMask.find('!') != -1)
	from = new Person(cnx->bot, fromMask);
   }
   
   String command = st.nextToken();
   String rest = st.rest();
   
   for (int i = 0; functions[i].name != 0; i++)
     if (command == functions[i].name) {
	functions[i].function(cnx, from, rest);
	break;
     }

   TelnetSpy::spyLine(line);
   
   delete from;
}

void
Parser::parse001(ServerConnection * cnx,
                 Person *from, String rest)
{
  String temp = "";
  StringTokenizer st(rest);
  String realNick = st.nextToken();

  if ((cnx->bot->nickName).toLower() != realNick) {
    // Yes, this can happen, and it was a very subtle bug :(
    cnx->bot->nickName = realNick;
    cnx->bot->userList->removeFirst();
    cnx->bot->userList->addUserFirst(realNick + "!" + cnx->bot->userHost, "*", 0, 3, realNick, -1, -1, "");
    cnx->bot->lastNickNameChange = time(0);
    cnx->bot->rehash();
  }
   
   cnx->bot->connected = true;

   cnx->queue->sendUserMode(cnx->bot->nickName, "+i");

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

  if ((cnx->bot->nickName).toLower() == on) {
    cnx->bot->userList->removeFirst();
    cnx->bot->userList->addUserFirst(nn + "!" + cnx->bot->userHost, "*", 0, 3, nn, -1, -1, "");
    cnx->bot->lastNickNameChange = time(0);
    cnx->bot->nickName = nn;
    cnx->bot->rehash();
  }

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

  if (command == "PING") {
    StringTokenizer st3(rest);
    rest = st3.nextToken();
    String c = st3.rest();
    if (cnx->bot->channelList->getChannel(c) &&
        cnx->bot->channelList->getChannel(c)->getUser(nick) &&
        cnx->bot->channelList->getChannel(c)->getUser(nick)->getAop() &&
        !(cnx->bot->channelList->getChannel(c)->getUser(nick)->mode & User::OP_MODE)
        && cnx->bot->channelList->getChannel(c)->getUser(nick)->userkey == rest)
      cnx->queue->sendChannelMode(c, "+o", nick);
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

  if (++(cnx->bot->ignoredUserhosts[fromUserhost])
      > Bot::MAX_MESSAGES) {
    if (cnx->bot->ignoredUserhosts[fromUserhost]
        == Bot::MAX_MESSAGES+1) {
      cnx->bot->ignoredUserhosts[fromUserhost] += Bot::IGNORE_DELAY;
      cnx->bot->logLine(from->getAddress() +
                   " is flooding me. We will ignore him/her/it.");
      if (!Utils::isChannel(to))
        from->sendNotice(String("Stop flooding me. You are now being ignored for ") +
                         String((long)Bot::IGNORE_DELAY) + " seconds.\002");
    }
    // The following lines reset the counter if you use the
    // command "!sorry" (if '!' is your command char).
    // This is not documented, I know. But one probably does
    // not want that every users can bypass the flood control
    // Of course, if you want this feature to remain 'secret',
    // do not use it in public.
    if (rest.toUpper() == String(cnx->bot->commandChar) + "SORRY") {
      cnx->bot->ignoredUserhosts[fromUserhost] = 0;
      from->sendNotice("\002Don't do it again!\002");
    }
    return;
  }

  if (rest[0] == '\001') {
    rest = rest.subString(1, rest.length() - 2);
    if (!Utils::isChannel(to))
    Parser::parseCTCP(cnx, from, to, rest);
  }
  else {
    if ((rest.length() < 5 ||
        rest.subString(1, 5).toUpper() != "IDENT") &&
       (rest.length() < 8 ||
        rest.subString(1, 8).toUpper() != "PASSWORD") &&
        !Utils::isChannel(to))
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
//  cnx->lag = (cnx->lag + 2 * (time(NULL) - cnx->pingTime)) / 3;
   cnx->lag = time(NULL) - cnx->pingTime;
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

// CTCPFunctionList - Table of CTCP commands
struct CTCPFunctionsStruct CTCPFunctionsInit[] =
{
     { "ACTION",	CTCP::Action,		false,	true,
	  "" },
     { "CLIENTINFO",	CTCP::ClientInfo,	false,	false,
	  "[<string query>]" },
     { "DCC",		CTCP::DCC,		false,	false,
	  "<string type> [<string argument>] [<int address>] [<int port>] [<int size>]" },
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
	  User::FRIEND,      	false 
     },
     { "ALIAS",      	UserCommands::Alias,       
	  User::MASTER,       	false 
     },
//     { "BAN",         	UserCommands::Ban,         User::TRUSTED_USER, true, false  },
     { "BANLIST",     	UserCommands::BanList,     
	  User::USER,        	true 
     },
     { "BEEP",		UserCommands::Beep,	   
	  User::TRUSTED_USER, 	false 
     },
//     { "CHANGELEVEL", UserCommands::ChangeLevel, User::MASTER,       false, false },
     { "CYCLE",       	UserCommands::Cycle,       
	  User::FRIEND,       	true 
     },
     { "DCCLIST",     	UserCommands::DCCList,     
	  User::MASTER,       	false 
     },
//     { "DEBAN",       	UserCommands::Deban,       User::TRUSTED_USER, true, false  },
     { "DELSERVER",   	UserCommands::DelServer,   
	  User::FRIEND,       	false 
     },
     { "DELUSER",     	UserCommands::DelUser,     
	  User::MASTER,       	false 
     },
     { "DEOP",        	UserCommands::Deop,        
	  User::TRUSTED_USER, 	true 
     },
     { "DIE",         	UserCommands::Die,         
	  User::MASTER,       	false 
     },
     { "DO",		UserCommands::Do,	   
	  User::USER,	       	true  
     },
     { "HELP",        	UserCommands::Help,        
	  User::NONE,         	false 
     },
//     { "IDENT",       	UserCommands::Ident,       User::NONE,         true, false  },
     { "INFO",		UserCommands::Info,	   
	  User::USER,	       	false 
     },
     { "INVITE",      	UserCommands::Invite,      
	  User::TRUSTED_USER, 	true  
     },
     { "JOIN",        	UserCommands::Join,        
	  User::MASTER,       	false 
     },
//     { "KEEP",        	UserCommands::Keep,        User::FRIEND,       true, false  },
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
     { "MSG",         	UserCommands::Msg,         
	  User::MASTER,       	false 
     },
     { "NAMES",       	UserCommands::Names,       
	  User::USER,         	true
     },
     { "NEXTSERVER",  	UserCommands::NextServer,  
	  User::FRIEND,       	false 
     },
//     { "NICK",        	UserCommands::Nick,        User::FRIEND,       false, false },
     { "NOTE",		UserCommands::Note,	   
	  User::USER,		false
     },
     { "NSLOOKUP",    	UserCommands::NsLookup,    
	  User::USER,         	false 
     },
     { "OP",          	UserCommands::Op,          
	  User::TRUSTED_USER, 	true  
     },
     { "PART",        	UserCommands::Part,        
	  User::MASTER,       	true  
     },
//     { "PASSWORD",    	UserCommands::Password,    User::USER,         true, false  },
     { "RAW",		UserCommands::Raw,	   
	  User::MASTER,       	false 
     },
     { "RECONNECT",   	UserCommands::Reconnect,   
	  User::MASTER,       	false 
     },
     { "SAVE",        	UserCommands::Save,        
	  User::MASTER,       	false 
     },
     { "SAY",         	UserCommands::Say,         
	  User::USER,         	true 
     },
     { "SERVER",      	UserCommands::Server,      
	  User::FRIEND,       	false 
     },
     { "SERVERLIST",  	UserCommands::ServerList,  
	  User::FRIEND,       	false 
     },
     { "STATS",		UserCommands::Stats,	   
	  User::USER,	       	false 
     },
//     { "TBAN",        	UserCommands::TBan,        User::TRUSTED_USER, true, false  },
//     { "TKBAN",       	UserCommands::TKBan,       User::USER,         true, false  },
     { "TEST",		UserCommands::Test,	   
	  User::MASTER,       	true  
     },
     { "TOPIC",       	UserCommands::Topic,       
	  User::TRUSTED_USER, 	true 
     },
     { "USERLIST",	UserCommands::UserList,	   
	  User::FRIEND,       	false 
     },
     { "",	0,	0,	false }
};

void Parser::parseMessage(ServerConnection *cnx, Person *from, String to,
			  String parameters)
{
   if (parameters[0] != cnx->bot->commandChar)
     return;
   
   StringTokenizer st(parameters);
   
   String command = st.nextToken().subString(1).toUpper();
   String rest = st.rest().trim();
   int level;
   bool identified = false;
   
   for (list<userFunction *>::iterator it = cnx->bot->userFunctions.begin();
	it != cnx->bot->userFunctions.end();
	++it)
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

