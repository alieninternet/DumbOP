
#include "serverqueue.h"
#include "serverqueueitem.h"

ServerQueueItem::ServerQueueItem(int pr, int pen, int t)
  : priority(pr), penalty(pen), type(t)
{ }



ServerQueueOtherItem::ServerQueueOtherItem(String l, int pr,
                                           int pen, int t)
  : ServerQueueItem(pr, pen, t), line(l)
{ }

String
ServerQueueOtherItem::getLine()
{
  return line;
}



ServerQueueChannelModeItem::ServerQueueChannelModeItem(String c, String m, String p)
  : ServerQueueItem(CHANNELMODE_PRIORITY,
                    CHANNELMODE_PENALTY,
                    CHANNELMODE),
    channel(c), mode(m), parameters(p)
{
  StringTokens st(p);
  paramcount = st.countTokens();
}

bool
ServerQueueChannelModeItem::merge(ServerQueueItem *sqi)
{
  if (sqi->type != CHANNELMODE)
    return false;

  ServerQueueChannelModeItem * sqcmi = (ServerQueueChannelModeItem *)sqi;

  if (sqcmi->channel != channel || (paramcount + sqcmi->paramcount > 3))
    return false;

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
      if (othersign == sign)
        mode = mode + String(c);
      else {
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

String
ServerQueueChannelModeItem::getLine()
{
  return String("MODE ") + channel + " " + mode + " " + parameters;
}



ServerQueueKickItem::ServerQueueKickItem(String c, String w, String r)
  : ServerQueueItem(KICK_PRIORITY,
                    KICK_PENALTY,
                    KICK),
    channel(c), who(w), reason(r), count(1)
{ }

bool
ServerQueueKickItem::merge(ServerQueueItem *sqi)
{
  if (sqi->type != KICK)
    return false;
  ServerQueueKickItem * sqki = (ServerQueueKickItem *)sqi;
  if (sqki->channel != channel)
    return false;
  if (count + sqki->count > 4)
    return false;
  who = who + "," + sqki->who;
  if (reason != sqki->reason)
    reason = "Mixed kick ! Top efficiency";
  penalty += sqki->penalty;
  count += sqki->count;

  return true;
}

String
ServerQueueKickItem::getLine()
{
  return String("KICK ") + channel + " " + who + " :" + reason;
}

ServerQueueNoticeItem::ServerQueueNoticeItem(String d, String m)
  : ServerQueueItem(NOTICE_PRIORITY,
                    NOTICE_PENALTY,
                    NOTICE),
    dest(d), message(m), count(1)
{ }

bool
ServerQueueNoticeItem::merge(ServerQueueItem *sqi)
{
  if (sqi->type != NOTICE)
    return false;
  ServerQueueNoticeItem * sqni = (ServerQueueNoticeItem *)sqi;
  if (sqni->message != message)
    return false;
  if (count + sqni->count > 4)
    return false;
  dest = dest + "," + sqni->dest;
  penalty += sqni->penalty;
  count += sqni->count;

  return true;
}

String
ServerQueueNoticeItem::getLine()
{
  return String("NOTICE ") + dest + " :" + message;
}
