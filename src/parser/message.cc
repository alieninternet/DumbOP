
#include "parser.h"
#include "usercommands.h"
#include "flags.h"
#include "bot.h"

// userFunctionsInit - Table of user functions
struct userFunctionsStruct userFunctionsInit[] = {
     { "ACCESS",	UserCommands::Access,      
	User::MASTER,		true 
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
	User::USER,		true
     },
     //     { "CHANGELEVEL", UserCommands::ChangeLevel, User::MASTER,       false, false },
     { "CLUE",		UserCommands::Clue,
	User::USER,		true
     },
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
	User::USER,         	false
     },
     { "IDENTIFY",     	UserCommands::Identify,
	User::NONE,         	false
     },
     { "INVITE",      	UserCommands::Invite,
	User::MANAGER, 		true // was trusted user
     },
     { "JOIN",        	UserCommands::Join,
	User::MASTER,       	false
     },
   //     { "KEEP",        	UserCommands::Keep,        User::MANAGER,       true, false  },
     { "KICK",        	UserCommands::Kick,
	User::MANAGER, 		true // was trusted user
     },
   //     { "KICKBAN",     	UserCommands::KickBan,     User::TRUSTED_USER, true, false  },
     { "LASTSEEN",	UserCommands::LastSeen,
	User::USER,	       	false
     },
     { "MODE",        	UserCommands::Mode,
	User::MANAGER, 		true // was trusted user
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
     { "PASSWORD",    	UserCommands::Password,
	User::USER,		false
     },
     { "PING",        	UserCommands::Ping,
	User::USER,       	false
     },
     { "RAW",		UserCommands::Raw,	   
	User::MASTER,       	false 
     },
     { "RECONNECT",   	UserCommands::Reconnect,   
	User::MASTER,       	false // master guy
     },
     { "REGISTER",   	UserCommands::Register,
	User::NONE,       	false
     },
     { "REPEAT",   	UserCommands::Repeat,
	User::USER,       	false
     },
     { "RULES",        	UserCommands::Rules,
	User::MASTER,       	true
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
	User::MANAGER,	       	false 
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
	User::MASTER,  		false // buggy
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
      /* If the command was not on a channel (private message) then we can
       * consider that it might be a command, only the user has not prepended
       * with the command char
       */
      if (Utils::isChannel(to)) {
	 // Get the channel information
	 Channel *c = cnx->bot->channelList->getChannel(to);
	 
	 // Make sure that actually works...
	 if (!c || !cnx->bot->wantedChannels[to]) {
	    return;
	 }
	 
	 // If it is a game channel, see if the game engine wants the data
	 if (cnx->bot->wantedChannels[to]->flags & CHANFLAG_ALLOW_GAMES) {
	    if (cnx->bot->games->parseLine(c, from, parameters)) {
	       return;
	    }
	 }
	 
	 /* Nothing more to do, and we do not want to continue into the 
	  * command parser. If we got here, we did not need to parse this line
	  */
	 return;
      } else {
	 // Prepend the command with the appropriate command char?
      }
   }

   StringTokens st(parameters);
   
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
	       StringTokens st2(rest);
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
	    
	    if (Channel *c = cnx->bot->channelList->getChannel(to)) {
	       u = c->getUser(from->getNick());
	    }
	    
	    if (!u || !u->userListItem) {
	       identified = true;
	    } else {
	       identified = u->userListItem->passwd == "" || u->userListItem->identified;
	    }
	 } else {
	    level = Utils::getLevel(cnx->bot, from->getAddress());
	    identified = true;
	 }
	 if ((level >= (*it)->minLevel) || ((*it)->minLevel == User::NONE)) {
	    cnx->bot->logLine(from->getAddress() + " did " + command);
	    (*it)->function(cnx, from, to, rest);
	    break;
	 } else {
	    if (!identified) {
	       from->sendNotice(String("You are not identified on channel \002") +
				to + String("\002 - Use \037") +
				String(cnx->bot->commandChar) +
				String("IDENTIFY\037 first."));
	    }
	 }
      }
   }
}
   
