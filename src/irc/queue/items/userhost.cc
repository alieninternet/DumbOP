
#include "config.h"
#include "serverqueueitem.h"

/* merge - Throw a userhost together. RFC2812 says we can only have 5 :(
 * Original 26/07/01, Simon Butcher <simonb@alien.net.au>
 */
bool ServerQueueUserhostItem::merge(ServerQueueItem *sqi)
{
   if (sqi->type != USERHOST) {
      return false;
   }
   
   ServerQueueUserhostItem *squi = (ServerQueueUserhostItem *)sqi;
   
   /* We want to be safe. We can have up to 500 chars on a line, but we gotta
    * be careful about this!
    */
   if ((count + squi->count) > 5) {
      return false;
   }
   
   // This list is space delimetered.
   nick = nick + " " + squi->nick;
   penalty += squi->penalty;
   count += squi->count;
   
   return true;
}


/* getLine - Create the line we are going to sent off
 * Original 26/07/01, Simon Butcher <simonb@alien.net.au>
 */
String ServerQueueUserhostItem::getLine()
{
   return String("USERHOST ") + nick;
}

/* ServerQueueUserhostItem - Sub-Class Constructor
 * Original 26/07/01, Simon Butcher <simonb@alien.net.au>
 */
ServerQueueUserhostItem::ServerQueueUserhostItem(String n)
: ServerQueueItem(USERHOST_PRIORITY,
		  USERHOST_PENALTY,
		  USERHOST),
  nick(n),
  count(1)
{ 
}

