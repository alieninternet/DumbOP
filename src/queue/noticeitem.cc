
#include "serverqueueitem.h"

bool ServerQueueNoticeItem::merge(ServerQueueItem *sqi)
{
   if (sqi->type != NOTICE) {
      return false;
   }
   
   ServerQueueNoticeItem * sqni = (ServerQueueNoticeItem *)sqi;
   
   if (sqni->message != message) {
      return false;
   }
   
   if (count + sqni->count > 4) {
      return false;
   }
   
   dest = dest + "," + sqni->dest;
   penalty += sqni->penalty;
   count += sqni->count;
   
   return true;
}

String ServerQueueNoticeItem::getLine()
{
   return String("NOTICE ") + dest + " :" + message;
}

ServerQueueNoticeItem::ServerQueueNoticeItem(String d, String m)
: ServerQueueItem(NOTICE_PRIORITY,
		  NOTICE_PENALTY,
		  NOTICE),
  dest(d), 
  message(m), 
  count(1)
{ 
}

