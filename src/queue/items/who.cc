
#include "serverqueueitem.h"

/* merge - Throw a who or two together to speed things up
 * Original 22/07/01, Simon Butcher <simonb@alien.net.au>
 */
bool ServerQueueWhoItem::merge(ServerQueueItem *sqi)
{
#ifdef ANAL_WHO_COMMAND // eg, no merge possible
   return false; /* :( */
#else
   if (sqi->type != WHO) {
      return false;
   }
   
   ServerQueueWhoItem *sqwi = (ServerQueueWhoItem *)sqi;
   
   if (count + sqwi->count > 10) {
      return false;
   }
   
   subject = subject + "," + sqwi->subject;
   penalty += sqwi->penalty;
   count += sqwi->count;
   
   return true;
#endif
}


/* getLine - Create the line we are going to sent off
 * Original 22/07/01, Simon Butcher <simonb@alien.net.au>
 */
String ServerQueueWhoItem::getLine()
{
   return String("WHO ") + subject;
}

/* ServerQueueWhoItem - Sub-Class Constructor
 * Original 22/07/01, Simon Butcher <simonb@alien.net.au>
 */
ServerQueueWhoItem::ServerQueueWhoItem(String s)
: ServerQueueItem(WHO_PRIORITY,
		  WHO_PENALTY,
		  WHO),
  subject(s), 
  count(1)
{ 
}

