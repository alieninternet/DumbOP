
#include "config.h"

#include <limits.h>

#include "serverqueue.h"
#include "bot.h"

ServerQueue::ServerQueue(ServerConnection *c, Socket *s)
: Queue(s, c->bot->debug),
  cnx(c),
  penalty(0)
{
   serverQueue.clear();
}

ServerQueue::~ServerQueue()
{
   penalty = INT_MIN;
   flush();
}

void ServerQueue::addItem(ServerQueueItem *sqi)
{
   // We want to keep track of how much we are sending out
   cnx->sentLen += (sqi->getLine().length() - 1);
   
   list<ServerQueueItem *>::iterator it, it2;
   
   for (it = serverQueue.begin(); it != serverQueue.end(); ++it) {
      if ((*it)->priority > sqi->priority) {
	 break;
      }
   }
   
   it2 = it; 
   --it2;
   
   if (it2 != serverQueue.end() && *it2) {
      // try to merge this item to the previous
      if ((*it2)->merge(sqi)) {
	 delete sqi;
	 return;
      }
   }
   
   serverQueue.insert(it, sqi);
}

void ServerQueue::addLine(String l, int pr, int pen, int t)
{
   ServerQueueOtherItem * sqoi =  new ServerQueueOtherItem(l, pr, pen, t);
   addItem(sqoi);
}

bool ServerQueue::flush()
{
   // Called every second, we decrement the penalty
   if (penalty > 0) {
      penalty--;
   }
   
   while (!serverQueue.empty() && (penalty < 5)) {
      ServerQueueItem * sqi = (*serverQueue.begin());
      penalty += sqi->penalty + sqi->getLine().length()/100 + 1;
      bool res = sendLine(sqi->getLine());
      
      serverQueue.erase(serverQueue.begin());
      delete sqi;
      
      if (!res) {
	 return false;
      }
   }
   
   return true;
}

