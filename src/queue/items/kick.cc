
#include "serverqueueitem.h"

ServerQueueKickItem::ServerQueueKickItem(String c, String w, String r)
: ServerQueueItem(KICK_PRIORITY,
		  KICK_PENALTY,
		  KICK),
  channel(c), 
  who(w), 
  reason(r), 
  count(1)
{ 
}

bool ServerQueueKickItem::merge(ServerQueueItem *sqi)
{
   if (sqi->type != KICK) {
      return false;
   }
   
   ServerQueueKickItem * sqki = (ServerQueueKickItem *)sqi;
   
   if (sqki->channel != channel) {
     return false;
   }
   
   if (count + sqki->count > 4) {
      return false;
   }
   
   who = who + "," + sqki->who;
   
   if (reason != sqki->reason) {
     reason = "Multi-kick";
   }
   
   penalty += sqki->penalty;
   count += sqki->count;
   
   return true;
}

String ServerQueueKickItem::getLine()
{
   return String("KICK ") + channel + " " + who + " :" + reason;
}

