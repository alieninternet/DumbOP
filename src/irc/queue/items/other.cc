
#include "config.h"
#include "serverqueueitem.h"

ServerQueueOtherItem::ServerQueueOtherItem(String l, int pr, int pen, int t)
: ServerQueueItem(pr, pen, t), 
  line(l)
{
}

String ServerQueueOtherItem::getLine()
{
   return line;
}



