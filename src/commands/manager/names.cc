
#include "commands.h"
#include "flags.h"
#include "utils.h"

/* Names - List names on a channel, or channels
 * Original 23/12/00, Pickle <pickle@alien.net.au>
 * Needs: Channel authorisation checking
 */
void Commands::Names(ServerConnection *cnx, Person *from,
			 String channel, String rest)
  {
     String nick = from->getNick();
     String result = "";
     int length = 0;
     
     if (rest == "-")
       for (map<String, Channel *, less<String> >::iterator it = cnx->bot->channelList->begin(); 
	    it != cnx->bot->channelList->end(); ++it) {
	  Channel *c = cnx->bot->channelList->getChannel((*it).first);
	  
	  if (true) {
	     result = "";
	     length = 0;
	     
	     from->sendNotice(String("Names on \002") + c->channelName + 
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
		   from->sendNotice(result);
		   result = ""; length = 0;
		}
	     }
	     
	     if (result != "")
	       from->sendNotice(result);
	  }
       }
     else {
	Channel *c = cnx->bot->channelList->getChannel(channel);

	from->sendNotice(String("Names on \002") + channel + "\002:");
	
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
	      from->sendNotice(result);
	      result = ""; length = 0;
	   }
	}
	
	if (result != "")
	  from->sendNotice(result);
     }
     
     from->sendNotice("\002End of names.\002");
  }
