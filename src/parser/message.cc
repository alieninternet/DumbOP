
#include "config.h"
#include "parser.h"
#include "commands.h"
#include "flags.h"
#include "bot.h"
#include "utils.h"

// userFunctionsInit - Table of user functions
struct userFunctionsStruct userFunctionsInit[] = {
     { "ACCESS",	Commands::Access,      
	User::MASTER,		true 
     },
     { "ADDSERVER",   	Commands::AddServer,   
	User::MASTER,      	false 
     },
     { "ALIAS",      	Commands::Alias,       
	User::MASTER,       	false 
     },
     { "ANSWER",      	Commands::Answer,
	User::MANAGER,       	true 
     },
//     { "BAN",         	Commands::Ban,         User::TRUSTED_USER, true, false  },
     { "BANLIST",     	Commands::BanList,     
	User::MASTER,        	true 
     },
     { "CATEGORY",	Commands::Category,
	User::USER,		true
     },
//     { "CHANGELEVEL", Commands::ChangeLevel, User::MASTER,       false, false },
     { "CLUE",		Commands::Clue,
	User::USER,		true
     },
     { "CYCLE",       	Commands::Cycle,       
	User::MASTER,       	true
     },
     { "DCCLIST",     	Commands::DCCList,     
	User::MASTER,       	false 
     },
//     { "DEBAN",       	Commands::Deban,       User::TRUSTED_USER, true, false  },
     { "DELSERVER",   	Commands::DelServer,   
	User::MASTER,       	false 
     },
//     { "DEOP",        	Commands::Deop,        
//	User::MASTER, 	true 
//     },
     { "DIE",         	Commands::Die,         
	User::MASTER,       	false 
     },
     { "DO",		Commands::Do,	   
	User::MASTER,	       	true  
     },
     { "GIVE",		Commands::Give,
	User::USER,	       	false
     },
     { "HELP",        	Commands::Help,        
	User::NONE,         	false 
     },
     { "HINT",        	Commands::Hint,        
	User::USER,         	false
     },
     { "IDENTIFY",     	Commands::Identify,
	User::NONE,         	false
     },
//     { "INVITE",      	Commands::Invite,
//	User::MANAGER, 		true // was trusted user
//     },
     { "JOIN",        	Commands::Join,
	User::MASTER,       	false
     },
//     { "KEEP",        	Commands::Keep,        User::MANAGER,       true, false  },
     { "KICK",        	Commands::Kick,
	User::MANAGER, 		true // was trusted user
     },
//     { "KICKBAN",     	Commands::KickBan,     User::TRUSTED_USER, true, false  },
     { "LASTSEEN",	Commands::LastSeen,
	User::USER,	       	false
     },
     { "MODE",        	Commands::Mode,
	User::MANAGER, 		true // was trusted user
     },
     { "NAMES",       	Commands::Names,
	User::MASTER,        	true
     },
     { "NEXTSERVER",  	Commands::NextServer,
	User::MANAGER,       	false
     },
//     { "NICK",        	Commands::Nick,        User::MANAGER,       false, false },
     { "NOTE",		Commands::Note,	   
	User::MASTER,		false
     },
     { "NSLOOKUP",    	Commands::NsLookup,    
	User::MASTER,        	false 
     },
     { "OP",          	Commands::Op,          
	User::TRUSTED_USER, 	true  
     },
     { "PART",        	Commands::Part,        
	User::MASTER,       	true  
     },
     { "PASSWORD",    	Commands::Password,
	User::USER,		false
     },
     { "PING",        	Commands::Ping,
	User::USER,       	false
     },
     { "RAW",		Commands::Raw,	   
	User::MASTER,       	false 
     },
     { "RECONNECT",   	Commands::Reconnect,   
	User::MANAGER,       	false
     },
     { "REGISTER",   	Commands::Register,
	User::NONE,       	false
     },
     { "REPEAT",   	Commands::Repeat,
	User::USER,       	false
     },
     { "RULES",        	Commands::Rules,
	User::NONE,       	true
     },   //  ^^^^ should be user?
     { "SAVE",        	Commands::Save,        
	User::MANAGER,       	false 
     },
     { "SAY",         	Commands::Say,         
	User::MASTER,        	true
     },
     { "SERVER",      	Commands::Server,      
	User::MANAGER,       	false 
     },
     { "SERVERLIST",  	Commands::ServerList,  
	User::MANAGER,       	false 
     },
     { "STATS",		Commands::Stats,	   
	User::USER,	       	false
     },
     { "TEST",		Commands::Test,	   
	User::MANAGER,       	false // THIS SHOULD BE MASTER, TRUE
     },
     { "TIME",       	Commands::Time,       
	User::USER, 		false
     },
     { "TOPIC",       	Commands::Topic,       
	User::TRUSTED_USER, 	true 
     },
     { "USERLIST",	Commands::UserList,	   
	User::MANAGER,       	false 
     },
     { "VOICE",		Commands::Voice,
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
   
