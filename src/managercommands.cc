
#include "commands.h"
#include "usercommands.h"
#include "str.h"
#include "stringtokenizer.h"
#include "utils.h"
#include "flags.h"

/* AddUser
 * Original 18/12/00, Pickle <pickle@alien.net.au>
 */
void
UserCommands::AddUser(ServerConnection *cnx, Person *from,
                      String channel, String rest)
{
  StringTokenizer st(rest);
  String mask, who, maskChannel, level, prot,
    nick, flags, passwd;

  mask = who = st.nextToken();
  maskChannel = st.nextToken();
  level = st.nextToken();
  prot = st.nextToken();
  nick = st.nextToken();
  flags = st.nextToken();
  passwd = st.nextToken();

  if (mask == "" || maskChannel == "" || level == "" ||
      prot == "" || nick == "") {
    from->sendNotice("\002Invalid syntax for this command.\002");
    return;
  }

  if (!Utils::isWildcard(mask)) {
    mask = cnx->bot->getUserhost(channel, who);
    if (mask == "") {
      from->sendNotice(String("\002I can not find\002 ") + who);
      return;
    }
    mask = Utils::makeWildcard(mask);
  }

  if (cnx->bot->userList->isInUserList(mask, maskChannel)) {
    from->sendNotice(who + " \002is already in userlist on channel(s)\002 " +
                     maskChannel);
    return;
  }

  int l, p;
  long f;

  l = atoi((const char *)level);
  if (l < 0 || l > User::MANAGER)
    return;
  if (l > Utils::getLevel(cnx->bot, from->getAddress())) {
    from->sendNotice("\002You can not give a level greater than yours.\002");
    return;
  }
  p = atoi((const char *)prot);
  if (p < 0 || p > User::NO_DEOP)
    return;

//  f = Utils::strToFlags(flags);
//
//  if (!f)
    f = 0;

  cnx->bot->userList->addUser(mask, maskChannel, l, p, nick, f, -1, passwd);

  from->sendNotice(String("\002Added\002 ") + mask +
                   " \002on channels\002 " + maskChannel);
  from->sendNotice(String("\002Level:\002 ") +
                   Utils::levelToStr(l) +
                   "  \002Protection:\002 " +
                   Utils::protToStr(p));

  cnx->bot->rehash();
}

/* AddServer
 * Original 18/12/00, Pickle <pickle@alien.net.au>
 */
void
UserCommands::AddServer(ServerConnection *cnx, Person *from,
                        String channel, String rest)
{
  if (rest.length() == 0) {
    from->sendNotice("\002You must supply a server name.\002");
    return;
  }

  StringTokenizer st(rest);
  String serverName = st.nextToken();
  int port = 6667;

  if (st.hasMoreTokens()) {
    String temp = st.nextToken();
    port = atoi((const char *)temp);
  }

  Message m = Commands::AddServer(cnx->bot, serverName, port);
  if (m.getCode() != 0)
    from->sendNotice(m.getMessage());
  else
    from->sendNotice(String("\002Server\002 ") +
                     serverName + " \002on port\002 " +
                     String((long)port) + " \002has been added "
                     "to the server list.\002");
}

/* Alias
 * Original 18/12/00, Pickle <pickle@alien.net.au>
 */
void
UserCommands::Alias(ServerConnection *cnx, Person *from,
                    String channel, String rest)
{
  StringTokenizer st(rest);
  String newF = st.nextToken().toUpper();
  String oldF = st.nextToken().toUpper();
  list<userFunction *>::iterator it;

  if (newF == "" || oldF == "") {
    from->sendNotice("\002Invalid syntax for this command.\002");
    return;
  }

  // First, we check that the "new" function does not exist
  for (it = cnx->bot->userFunctions.begin(); it !=
         cnx->bot->userFunctions.end(); ++it)
    if (newF == (*it)->name) {
      from->sendNotice(newF + " \002is already an alias.\002");
      return;
    }

  // Next, we check that the "old" function exist
  bool found = false;
  userFunction *u;
  for (it = cnx->bot->userFunctions.begin(); it !=
         cnx->bot->userFunctions.end(); ++it)
    if (oldF == (*it)->name) {
      found = true;
      u = *it;
      break;
    }
  if (!found) {
    from->sendNotice(String("\002I don't know the\002 ") + oldF +
                     " \002command.");
    return;
  }

  // Fine, we do the binding
  cnx->bot->userFunctions.push_back(new
                               userFunction((char *)(const char 
                                                            *)newF,
                                                   u->function,
                                                   u->minLevel,
                                                   u->needsChannelName));

  from->sendNotice("\002Alias added.\002");
}

/* DelServer
 * Original 18/12/00, Pickle <pickle@alien.net.au>
 */
void
UserCommands::DelServer(ServerConnection *cnx, Person *from,
                        String channel, String rest)
{
  if (rest.length() == 0) {
    from->sendNotice("\002You need to supply a server number"
                     " for this command.\002");
    return;
  }

  int serverNumber = atoi(rest);

  Message m = Commands::DelServer(cnx->bot, serverNumber);
  if (m.getCode() != 0)
    from->sendNotice(m.getMessage());
  else
    from->sendNotice(String("Deleted server ") +
                     cnx->bot->serverList->get(serverNumber)->getHostName() +
                     " (" + String((long)cnx->bot->serverList->get(serverNumber)->getPort()) +
                     ").");
}

/* DelUser
 * Original 18/12/00, Pickle <pickle@alien.net.au>
 */
void
UserCommands::DelUser(ServerConnection *cnx, Person *from,
                      String channel, String rest)
{
  StringTokenizer st(rest);

  String who;
  String mask = who = st.nextToken();
  String maskChannel = st.nextToken();

  if (mask == "" || maskChannel == "") {
    from->sendNotice("\002Invalid syntax for this command.\002");
    return;
  }

  if (!Utils::isWildcard(mask)) {
    mask = cnx->bot->getUserhost(channel, who);
    if (mask == "") {
      from->sendNotice(String("\002I can not find\002 ") + who);
      return;
    }
    mask = Utils::makeWildcard(mask);
  }
  
  if (!cnx->bot->userList->isInUserList(mask, maskChannel)) {
    from->sendNotice(mask + " \002is not in userlist on channel(s)\002 " +
                     maskChannel);
    return;
  }

  cnx->bot->userList->removeUser(mask, maskChannel);
  from->sendNotice(who + " \002has been removed from the userlist.\002");
  cnx->bot->rehash();
}

/* Die - Make the bot quit
 * Original 13/12/00, Pickle <pickle@alien.net.au>
 * 28/12/00 Pickle - Changed default quit message to VERSION_STRING
 */
void UserCommands::Die(ServerConnection *cnx, Person *from,
		       String channel, String rest)
{
   if (rest.length() == 0)
     Commands::Die(cnx->bot, from->getNick() +
		   String(" requested me to leave [") + 
		   VERSION_STRING + String("]"));
   else
     Commands::Die(cnx->bot, rest);
}

/* Do
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 * Needs: Remove command.c dependancy
 */
void UserCommands::Do(ServerConnection *cnx, Person *from,
		      String channel, String rest)
{
   if (!rest.length())
     from->sendNotice("And do what?");
   else {
      
      Message m = Commands::Action(cnx->bot, channel, rest);
      if (m.getCode() != 0)
	from->sendNotice(m.getMessage());
   }
}

/* Names - List names on a channel, or channels
 * Original 23/12/00, Pickle <pickle@alien.net.au>
 * Needs: Channel authorisation checking
 */
void UserCommands::Names(ServerConnection *cnx, Person *from,
			 String channel, String rest)
  {
     String nick = from->getNick();
     String result = "";
     int length = 0;
     
     if (rest == "-")
       for (map<String, Channel *, less<String> >::iterator it = cnx->bot->channelList->begin(); 
	    it != cnx->bot->channelList->end(); ++it) {
	  Channel *c = cnx->bot->channelList->getChannel((*it).first);
	  
	  if (true) {
	     result = "";
	     length = 0;
	     
	     from->sendNotice(String("Names on \002") + c->channelName + 
			      "\002:");
	     
	     for (map<String, User *, less<String> >::iterator itB = 
		  c->channelMemory.begin();
		  itB != c->channelMemory.end(); ++itB) {
		result = result +
		  ((((*itB).second->mode & User::OP_MODE) ?
		    "@" :
		    (((*itB).second->mode & User::VOICE_MODE) ? 
		     "+" : ""))) +
		  (((*itB).second->userListItem) ?
		   "\037" : "" ) +
		  (*itB).second->nick +
		  (((*itB).second->userListItem) ?
		   "\037" : "") + " ";
		length += (*it).first.length() + 1;
		if (length >= 350) {
		   from->sendNotice(result);
		   result = ""; length = 0;
		}
	     }
	     
	     if (result != "")
	       from->sendNotice(result);
	  }
       }
     else {
	Channel *c = cnx->bot->channelList->getChannel(channel);

	from->sendNotice(String("Names on \002") + channel + "\002:");
	
	for (map<String, User *, less<String> >::iterator it = 
	     c->channelMemory.begin();
	     it != c->channelMemory.end(); ++it) {
	   result = result +
	     ((((*it).second->mode & User::OP_MODE) ?
	       "@" :
	       (((*it).second->mode & User::VOICE_MODE) ? 
		"+" : ""))) +
	     (((*it).second->userListItem) ?
	      "\037" : "" ) +
	     (*it).second->nick +
	     (((*it).second->userListItem) ?
	      "\037" : "") + " ";
	   length += (*it).first.length() + 1;
	   if (length >= 350) {
	      from->sendNotice(result);
	      result = ""; length = 0;
	   }
	}
	
	if (result != "")
	  from->sendNotice(result);
     }
     
     from->sendNotice("\002End of names.\002");
  }

/* Merge with Server */
void
UserCommands::NextServer(ServerConnection *cnx, Person *from,
                         String channel, String rest)
{
  if (cnx->bot->serverList->size() == 0) {
    from->sendNotice("\002Server list is empty !\002");
    return;
  }

  if (cnx->bot->serverList->size() == 1) {
    from->sendNotice("\002Server list has only one item. Use"
                     " \"reconnect\" to force reconnection.\002");
    return;
  }

  if (cnx->bot->canChangeServer()) {
    cnx->queue->sendQuit("Changing server");
    cnx->bot->nextServer();
  }
  else
    from->sendNotice("\002I can not change server without"
                     " losing op on a channel I am on.\002");
}

/* Nick - Change nickname
 * Original 14/12/00, Pickle <pickle@alien.net.au>
 */
void
UserCommands::Nick(ServerConnection *cnx, Person *from,
                   String channel, String rest)
{
  StringTokenizer st(rest);
  String nick = st.nextToken();

  if (rest == "") {
    from->sendNotice(String("\002No nickname given.\002"));
    return;
  }

  if (!Utils::isValidNickName(nick)) {
    from->sendNotice(String("\002") + nick +
                     " is not a valid nickname\002");
    return;
  }

  cnx->bot->wantedNickName = nick;
  cnx->queue->sendNick(nick);
}

/* Save - Force a database save
 * Original 15/12/00, Pickle <pickle@alien.net.au>
 */
void UserCommands::Save(ServerConnection *cnx, Person *from,
			String channel, String rest)
{
   cnx->bot->userList->save();
   cnx->bot->noteList->save();
   from->sendNotice("Databases saved.");
}

/* Say
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 */
void UserCommands::Say(ServerConnection *cnx, Person *from,
		       String channel, String rest)
{
   if (!rest.length())
     from->sendNotice("And say what?");
   else
     cnx->queue->sendPrivmsg(channel,rest);
}

/* Server
 * Original 14/12/00, Pickle <pickle@alien.net.au>
 * Needs: Join serverlist and nextserver and reconnect into this command
 */
void UserCommands::Server(ServerConnection *cnx, Person *from,
                     String channel, String rest)
{
  int serverNumber;
  String nick = from->getNick();

  if (rest.length() == 0) {
    from->sendNotice("\002You need to supply a server number for this command.\002");
    return;
  }

  serverNumber = atoi(rest);

  if (serverNumber < 0 || serverNumber >= cnx->bot->serverList->size()) {
    from->sendNotice(String((long)serverNumber) +
                      " \002is an invalid server number (see the serverlist).\002");
    return;
  }

  if (cnx->bot->canChangeServer()) {
    cnx->queue->sendQuit("Changing server");
    cnx->queue->flush();
    cnx->bot->connect(serverNumber);
  }
  else
    from->sendNotice("\002I can not change server without losing op on a channel I am on.\002");
}

/* merge with server */
void
UserCommands::ServerList(ServerConnection *cnx, Person *from,
                     String channel, String rest)
{
  String nick = from->getNick();
  
  from->sendNotice("\002Server list:\002");
  long l = 0;
  String s;
  for (vector<class Server *>::iterator it =
         cnx->bot->serverList->v.begin();
       it != cnx->bot->serverList->v.end();
       ++it) {
    s = (*it) == cnx->server ? "\026" : "";
    from->sendNotice(s + String(l++) + s + " " +
                     (*it)->getHostName() +
                     " (" + String((long)((*it)->getPort())) +
                     ")");
  }
  from->sendNotice("\002End of server list.\002");
}

/* Stats - Show a list of statistical mumbo jumbo to impress stupid people
 * Original 19/12/00, Pickle <pickle@alien.net.au>
 */
void UserCommands::Stats(ServerConnection *cnx, Person *from,
			 String channel, String rest)
{
   Version::sendInformation(cnx, from);

   if (rest.length() != 0) {
      from->sendNotice(String("Statistics for \002") + rest +
		       String("\002:"));
      from->sendNotice("This command needs to be finished.");
   } else {
      int num_chans, num_users, num_bots, num_chanowns, num_suspend;
      int num_ident, num_online, num_chanppl, num_chanops, num_chans_on;
      int num_lvluser, num_lvltrusted, num_lvlfriend, num_lvlmaster;
      int num_chanvoiced, num_nopass, peak_chanppl, peak_chanops;
      int peak_chanvoiced, num_dcc_con, num_chanbans, peak_chanbans;
      time_t uptime = cnx->bot->currentTime.time - cnx->bot->startTime;
      
      // This is a scarey way of zeroing every variable!
      num_chans = num_users = num_bots = num_chanowns = num_suspend = 
	num_ident = num_online = num_chanppl, num_chanops = num_chans_on =
	num_lvluser = num_lvltrusted = num_lvlfriend = num_lvlmaster = 
	num_chanvoiced = num_nopass = peak_chanppl = peak_chanops =
	peak_chanvoiced = num_dcc_con = num_chanbans = peak_chanbans = 0;
      
      for (list<UserListItem *>::iterator it = cnx->bot->userList->l.begin();
	   it != cnx->bot->userList->l.end();
	   it++) {
	 num_users++;
	 if ((*it)->flags & PERSONFLAG_IS_BOT)
	   num_bots++;
	 if ((*it)->flags & USERFLAG_IS_CHAN_OWNER)
	   num_chanowns++;
	 if ((*it)->flags & USERFLAG_SUSPENDED)
	   num_suspend++;
	 if ((*it)->flags & PERSONFLAG_IDENTIFIED)
	   num_ident++;
	 if ((*it)->lastseen == 0)
	   num_online++;
	 switch ((*it)->level) {
	  case 1: num_lvluser++;
	    break;
	  case 2: num_lvltrusted++;
	    break;
	  case 3: num_lvlfriend++;
	    break;
	  case 4: num_lvlmaster++;
	    break;
	 }
	 if ((*it)->passwd == "")
	   num_nopass++;
      }
      
      for (map<String, Channel *, less<String> >::iterator it = cnx->bot->channelList->begin(); 
	   it != cnx->bot->channelList->end(); ++it) {
	 Channel *chan = cnx->bot->channelList->getChannel((*it).first);
	 
	 num_chans++;
	 num_chanppl += chan->count;
	 num_chanops += chan->countOp;
	 num_chanvoiced += chan->countVoice;
	 peak_chanppl += chan->count_peak;
	 peak_chanops += chan->countOp_peak;
	 peak_chanvoiced += chan->countVoice_peak;
	 if (cnx->bot->channelList->getChannel((*it).first)->joined)
	   num_chans_on++;
	 for (vector<BanEntry *>::iterator itB = chan->channelBanlist.begin();
	      itB != chan->channelBanlist.end(); ++itB)
	   num_chanbans++;
//	 peak_chanbans += chan->countBans_peak;
      }
      
      for (list<DCCConnection *>::iterator it = cnx->bot->dccConnections.begin();
	   it != cnx->bot->dccConnections.end(); ++it) {
	 num_dcc_con++;
      }
      
      from->sendNotice(String("\002\037General Statistics:\037\002"));
      from->sendNotice(String("Name: ").prepad(10) +
		       String(cnx->bot->nickName + " (" +
			      cnx->bot->wantedNickName + ")").pad(23) +
		       String("  Lag Check: ") +
		       Utils::timeBigToStr(cnx->lag));
#ifdef DEBUG
      from->sendNotice(String("Up time: ").prepad(10) +
		       (!(cnx->bot->debug) ?
			Utils::timelenToStr(uptime) :
			(Utils::timelenToStr(uptime).pad(45) +
			 String("\026 DEBUG MODE \026"))));
#else
      from->sendNotice(String("Up time: ").prepad(10) +
		       Utils::timelenToStr(uptime));
#endif
      from->sendNotice(String("IRC data received: ").prepad(20) +
		       (String((long)(cnx->receivedLen / 1024)) +
			String("k")).pad(15) +
		       String("IRC data sent: ").prepad(20) +
		       (String((long)(cnx->sentLen / 1024)) +
			String("k")));

      /*
       from->sendNotice(String("\002\037Number of users:\037 ") + 
			String(num_users) + String(" (") + 
			String(num_online) + String(" currently online)\002"));
       from->sendNotice(String("Number of Bots: ").prepad(18) +
			String(num_bots).pad(6) +
			String("Channel Admins: ").prepad(18) +
			String(num_chanowns).pad(6) +
			String("Suspended: ").prepad(18) +
			String(num_suspend));
       from->sendNotice(String("Normal Users: ").prepad(18) +
			String(num_lvluser).pad(6) +
			String("Trusted Users: ").prepad(18) +
			String(num_lvltrusted).pad(6) +
			String("Bot Friends: ").prepad(18) +
			String(num_lvlfriend));
       from->sendNotice(String("Bot Masters: ").prepad(18) +
			String(num_lvlmaster).pad(6) +
			String("Identified Users: ").prepad(18) +
			String(num_ident).pad(6) +
			String("Users w/o passwd: ").prepad(18) +
			String(num_nopass));

       from->sendNotice(String("\002\037Number of channels:\037 ") + 
			String(num_chans) + String(" (") +
			String(num_chans_on) + 
			String(" currently active)\002"));
       from->sendNotice(String("Total People: ").prepad(16) +
			(String(num_chanppl) + String(" (Peak: ") +
			 String(peak_chanppl) + String(")")).pad(19) +
			String("Total Banned: ").prepad(16) +
			(String(num_chanbans) + String(" (Peak: ") +
			 String(peak_chanbans) + String(")")));
       from->sendNotice(String("Total Opped: ").prepad(16) +
			(String(num_chanops) + String(" (Peak: ") +
			 String(peak_chanops) + String(")")).pad(19) +
			String("Total Voiced: ").prepad(16) +
			(String(num_chanvoiced) + String(" (Peak: ") +
			 String(peak_chanvoiced) + String(")")));

       from->sendNotice(String("\002\037Number of DCC Fileserver Connections:\037 ") +
			String(num_dcc_con) + String("\002"));
       */
 
   }
   from->sendNotice(String("\002End of statistics.\002"));
}

/* Raw - Write a raw line to the server
 * Original - 11/12/00, Pickle <pickle@alien.net.au>
 */
void UserCommands::Raw(ServerConnection *cnx, Person *from,
		       String channel, String rest)
  {
     if (rest.length() != 0)
       Commands::Do(cnx->bot, rest);
  }

/* merge with server */
void
 UserCommands::Reconnect(ServerConnection *cnx, Person *from,
                        String channel, String rest)
{
  String nick = from->getNick();

  if (cnx->bot->canChangeServer()) {
    cnx->queue->sendQuit("Reconnecting");
    cnx->bot->reconnect();
  }
  else
    from->sendNotice("\002I can not change server without losing op on a channel I am on.\002");
}

/* UserList - Display a list of users
 * Original 14/12/00, Pickle <pickle@alien.net.au>
 * 22/12/00 Pickle - Reformatted
 */
void UserCommands::UserList(ServerConnection *cnx, Person *from,
			    String channel, String rest)
  {
     int num = 0;
     
     from->sendNotice("\002Userlist:\002");
     from->sendNotice("\026 \037Nickname       \037 \037Mask            \037 \037Channel   \037 \037Level       \037 \037Flags     \037 \026");
     
     for (list<UserListItem *>::iterator it = cnx->bot->userList->l.begin();
	  it != cnx->bot->userList->l.end();
	  it++) {
	from->sendNotice((((*it)->lastseen == 0) ?
			  (((*it)->flags & PERSONFLAG_IDENTIFIED) ?
			   String("\002*\002") :
			   String("\002+\002")) : 
			  String(" ")) +
			 Utils::getFirstNick((*it)->nick).prepad(15) + " " +
			 String("\002[\002") +
			 (*it)->mask.getMask().pad(14) + String("\002]\002 ") +
			 (*it)->channelMask.getMask().pad(10) + " " +
			 Utils::levelToStr((*it)->level).pad(12) + " " +
			 Utils::flagsToStr((*it)->flags).pad(10) + " " +
			 String((*it)->flags));
	num++;
     }
     
     if (num == 0) 
       from ->sendNotice("\002End of userlist\002 (No matches)");
     else 
       from->sendNotice(String("\002End of userlist\002 (") +
			String((int)num) + " entries)");
  }


