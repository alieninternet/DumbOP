
#include "serverqueueitem.h"

/* merge - Throw an ison together. The RFC says we can do HEAPS of them!!
 * Original 26/07/01, Simon Butcher <simonb@alien.net.au>
 */
bool ServerQueueIsonItem::merge(ServerQueueItem *sqi)
{
   if (sqi->type != ISON) {
      return false;
   }
   
   ServerQueueIsonItem *sqii = (ServerQueueIsonItem *)sqi;
   
   /* We want to be safe. We can have up to 500 chars on a line, but we gotta
    * be careful about this!
    */
   if ((nick.length() + sqii->nick.length()) >= 450) {
      return false;
   }
   
   // This list is space delimetered.
   nick = nick + " " + sqii->nick;
   penalty += sqii->penalty;
   
   return true;
}


/* getLine - Create the line we are going to sent off
 * Original 26/07/01, Simon Butcher <simonb@alien.net.au>
 */
String ServerQueueIsonItem::getLine()
{
   return String("ISON ") + nick;
}

/* ServerQueueIsonItem - Sub-Class Constructor
 * Original 26/07/01, Simon Butcher <simonb@alien.net.au>
 */
ServerQueueIsonItem::ServerQueueIsonItem(String n)
: ServerQueueItem(ISON_PRIORITY,
		  ISON_PENALTY,
		  ISON),
  nick(n)
{ 
}

