/* src/irc/commands/manager/names.cc
 * NAMES command
 * Copyright (c) 2000 Alien Internet Services
 */

#include "config.h"

#include "commands.h"
#include "flags.h"
#include "utils.h"


/* Names - List names on a channel, or channels
 * Original 23/12/00, Pickle <pickle@alien.net.au>
 * Needs: Channel authorisation checking
 */
void Commands::Names(Person *from,
			 String channel, String rest)
  {
     String nick = from->getNick();
     String result = "";
     int length = 0;
     
     if (rest == "-")
       for (map<String, Channel *, less<String> >::iterator it = from->cnx->bot->channelList->begin(); 
	    it != from->cnx->bot->channelList->end(); ++it) {
	  Channel *c = from->cnx->bot->channelList->getChannel((*it).first);
	  
	  if (true) {
	     result = "";
	     length = 0;
	     
	     from->sendLine(String("Names on \002") + c->channelName + 
			      "\002:");
	     
	     for (map<String, User *, less<String> >::iterator itB = 
		  c->channelMemory.begin();
		  itB != c->channelMemory.end(); ++itB) {
		result = result +
		  ((((*itB).second->mode & User::OP_MODE) ?
		    "@" :
		    (((*itB).second->mode & User::VOICE_MODE) ? 
		     "+" : ""))) +
		  (((*itB).second->userListItem) ?
		   "\037" : "" ) +
		  (*itB).second->nick +
		  (((*itB).second->userListItem) ?
		   "\037" : "") + " ";
		length += (*it).first.length() + 1;
		if (length >= 350) {
		   from->sendLine(result);
		   result = ""; length = 0;
		}
	     }
	     
	     if (result != "")
	       from->sendLine(result);
	  }
       }
     else {
	Channel *c = from->cnx->bot->channelList->getChannel(channel);

	from->sendLine(String("Names on \002") + channel + "\002:");
	
	for (map<String, User *, less<String> >::iterator it = 
	     c->channelMemory.begin();
	     it != c->channelMemory.end(); ++it) {
	   result = result +
	     ((((*it).second->mode & User::OP_MODE) ?
	       "@" :
	       (((*it).second->mode & User::VOICE_MODE) ? 
		"+" : ""))) +
	     (((*it).second->userListItem) ?
	      "\037" : "" ) +
	     (*it).second->nick +
	     (((*it).second->userListItem) ?
	      "\037" : "") + " ";
	   length += (*it).first.length() + 1;
	   if (length >= 350) {
	      from->sendLine(result);
	      result = ""; length = 0;
	   }
	}
	
	if (result != "")
	  from->sendLine(result);
     }
     
     from->sendLine("\002End of names.\002");
  }
