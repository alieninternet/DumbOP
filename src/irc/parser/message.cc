
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
     { "CHECKPOINT",   	Commands::Checkpoint,
	User::MANAGER,       	false 
     },
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
     { "INVENTORY",    	Commands::Inventory,
	User::USER,         	false
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
     { "NOTE",		Commands::Note,	   
	User::MASTER,		false
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
void Parser::parseMessage(Person *from, String to, String parameters)
{
   // Rip the command apart
   StringTokens st(parameters);
   String command = st.nextToken().toUpper();
   String rest = st.rest().trim();
   
   // Was this command sent to a channel?
   if (Utils::isChannel(to)) {
      // Get the channel information
      Channel *c = from->cnx->bot->channelList->getChannel(to);
     
      // Make sure that the channel pointer actually works...
      if (!c || !from->cnx->bot->wantedChannels[c->channelName]) {
	 return;
      }
      
      /* If this command was sent to the channel, the channel must allow
       * the commands and the command must have the command prefix char.
       * If not, we do not let it through the parser, but pass it onto other
       * parsing routines instead.
       */
      if (command[0] != from->cnx->bot->commandChar) {
	 // No command char, could be needed for other things..
	 
	 // If it is a game channel, see if the game engine wants the data
	 if (from->cnx->bot->wantedChannels[c->channelName]->flags &
	     CHANFLAG_ALLOW_GAMES) {
	    if (from->cnx->bot->games->parseLine(c, from, parameters)) {
	       return;
	    }
	 }
	 
	 /* Nothing more to do, and we do not want to continue into the 
	  * command parser. If we got here, we did not need to parse this line
	  */
	 return;
      } else if (from->cnx->bot->wantedChannels[c->channelName]->flags &
		 CHANFLAG_DISABLE_COMMANDS) {
	 // This command has the command char, but channel commands are off
	 return;
      } else {
	 // A command, rip the command prefix char off
	 command = command.subString(1);
      }
   } else { // It was sent privately
      // Check if there is a command char we need to strip
      if (command[0] == from->cnx->bot->commandChar) {
	 command = command.subString(1);
      }
   }
   
   // Ready some information about this user..
   int level = 0;
   bool identified = true; // default to true for non-existant users...
   if (from->uli) {
      level = from->uli->level;
      identified = from->uli->identified;
   }

   // Run through the list of functions and look for a command that matches..
   for (list<userFunction *>::iterator it = 
	from->cnx->bot->userFunctions.begin(); 
	it != from->cnx->bot->userFunctions.end(); ++it) {
      // Does this command match?
      if (command == (*it)->name) {
	 // If this command requires a channel specification, grab it
	 if ((*it)->needsChannelName) {
	    // Check if the channel was specified on the line
	    if (Utils::isChannel(rest)) {
	       StringTokens st2(rest);
	       to = st.nextToken();
	       rest = st.rest();
	    }

	    // Otherwise, check if the command was on a channel...
	    if (!Utils::isChannel(to)) {
	       from->sendLine("You need to specify a channel with this command");
	       return;
	    }
	    
	    // Make sure we are on the channel the command is for..
	    if (!from->cnx->bot->channelList->getChannel(to)) {
	       from->sendLine(String("I am not on \002") + to +
			      String("\002"));
	       return;
	    }
	 }
	 
	 // Can we let this user run function?
	 if ((identified && (level >= (*it)->minLevel)) ||
	     ((*it)->minLevel == User::NONE)) {
	    // Log what we are about to do..
	    from->cnx->bot->logLine(from->getAddress() + " did " + command);
	    
	    // Do the function
	    (*it)->function(from, to, rest);
	 } else {
	    if (!identified) {
	       // Tell the user they must identify themselves
	       from->sendLine(String("You are not identified; please use \037/msg ") +
			      from->cnx->bot->nickName + String(" ") +
			      String(from->cnx->bot->commandChar) +
			      String("IDENTIFY \002<password>\002\037."));
	    } else {
	       if (Utils::isChannel(to) &&
		   (from->cnx->bot->wantedChannels[to]->flags &
		    CHANFLAG_TALK_TO_NONREGISTERED) &&
		   ((*it)->minLevel == User::USER)) {
		  /* If they are talking to us, or on a channel where we can
		   * tell them to register... tell them to register!
		   */
		  from->sendLine(String("You have not registered; please type \037/msg ") +
				 from->cnx->bot->nickName + String(" ") +
				 String(from->cnx->bot->commandChar) +
				 String("HELP REGISTER\037 for more details."));
	       }
	    }
	 }

	 // Leave the for loop
	 break;
      }
   }
}
   
