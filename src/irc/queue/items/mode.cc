
#include "config.h"
#include "serverqueueitem.h"

ServerQueueChannelModeItem::ServerQueueChannelModeItem(String c, String m, 
						       String p)
: ServerQueueItem(CHANNELMODE_PRIORITY,
		  CHANNELMODE_PENALTY,
		  CHANNELMODE),
  channel(c), 
  mode(m), 
  parameters(p)
{
   StringTokens st(p);
   paramcount = st.countTokens();
}

bool ServerQueueChannelModeItem::merge(ServerQueueItem *sqi)
{
   if (sqi->type != CHANNELMODE) {
      return false;
   }
   
   ServerQueueChannelModeItem * sqcmi = (ServerQueueChannelModeItem *)sqi;
   
   if (sqcmi->channel != channel || (paramcount + sqcmi->paramcount > 3)) {
      return false;
   }
   
   char sign = '+';
   char c;
   
   for (int i = 0; i < mode.length(); i++) {
      c = mode[i];
      switch (c) {
       case '+':
       case '-':
	 sign = c;
      }
   }
   
   char othersign = '+';
   
   for (int i = 0; i < sqcmi->mode.length(); i++) {
      c = sqcmi->mode[i];
      switch (c) {
       case '+':
       case '-':
	 othersign = c;
	 break;
       default:
	 if (othersign == sign) {
	    mode = mode + String(c);
	 } else {
	    mode = mode + String(othersign) + String(c);
	    sign = othersign;
	 }
      }
   }
   
   parameters = parameters + " " + sqcmi->parameters;
   paramcount += sqcmi->paramcount;
   penalty += sqcmi->penalty;
   
   return true;
}

String ServerQueueChannelModeItem::getLine()
{
   return String("MODE ") + channel + " " + mode + " " + parameters;
}

