
#include "serverqueueitem.h"

/* merge - Throw a join or two together to speed things up
 * Original 22/07/01, Simon Butcher <simonb@alien.net.au>
 */
bool ServerQueueJoinItem::merge(ServerQueueItem *sqi)
{
   if (sqi->type != JOIN) {
      return false;
   }
   
   ServerQueueJoinItem *sqji = (ServerQueueJoinItem *)sqi;
   
   if (count + sqji->count > 10) {
      return false;
   }
   
   param = param + "," + sqji->param;
   penalty += sqji->penalty;
   count += sqji->count;
   
   return true;
}


/* getLine - Create the line we are going to sent off
 * Original 22/07/01, Simon Butcher <simonb@alien.net.au>
 */
String ServerQueueJoinItem::getLine()
{
   return String("JOIN ") + param;
}

/* ServerQueueJoinItem - Sub-Class Constructor
 * Original 22/07/01, Simon Butcher <simonb@alien.net.au>
 */
ServerQueueJoinItem::ServerQueueJoinItem(String p)
: ServerQueueItem(JOIN_PRIORITY,
		  JOIN_PENALTY,
		  JOIN),
  param(p), 
  count(1)
{ 
}

