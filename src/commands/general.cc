

#include "commands.h"
#include "flags.h"

#include "utils.h"

/* Help - Display help
 * Original 15/12/00, Pickle <pickle@alien.net.au>
 * 26/12/00 Pickle - Changed header output depending on command or topic
 */
void Commands::Help(ServerConnection *cnx, Person *from,
			String channel, String rest)
{
   if (rest.length() == 0) {
      int level = Utils::getLevel(cnx->bot, from->getAddress());
      String result = "\002Commands available to you are:\002 ";
      for (list<userFunction *>::iterator it = cnx->bot->userFunctions.begin(); 
	   it != cnx->bot->userFunctions.end(); ++it)
	if ((*it)->minLevel <= level) {
	   result = result + (*it)->name + " ";
	   if (result.length() >= 350) {
	      from->sendNotice(result);
	      result = "";
	   }
	}
      if (result != "")
	from->sendNotice(result);
      from->sendNotice(String("Use \002") + String(cnx->bot->commandChar) +
		       String("HELP <command>\002 for more information (eg. \002") +
		       String(cnx->bot->commandChar) + 
		       String("HELP HELP\002)."));
      return;
   }
   
   StringTokens st(rest);
   String command = st.nextToken().toUpper();
   ifstream helpFile(cnx->bot->helpFileName);
   
   if (!helpFile) {
      from->sendNotice("I can't find the help file :(");
      return;
   }
   
   String buf;
   while (!helpFile.eof()) {
      helpFile >> buf;
      if (buf.subString(0, command.length()) == String(":") + command) {
	 buf = buf.subString(1);
	 int level = -1;
	 
	 for (list<userFunction *>::iterator it = 
	      cnx->bot->userFunctions.begin(); 
	      it != cnx->bot->userFunctions.end(); ++it)
	   if ((*it)->name == command) {
	      level = (*it)->minLevel;
	      break;
	   }
	 
	 if (level > -1)
	   from->sendNotice(String(String("Help for command \002") +
				   String(cnx->bot->commandChar) +
				   command + String("\002:")).pad(40) + 
			    String("Access Required: \002") +
			    Utils::levelToStr(level) +
			    String("\002"));
	 else
	   from->sendNotice(String(String("Help on topic \002\"") + command +
				   String("\002\":")));
	 
	 while (buf != "") {
	    from->sendNotice(buf);
	    helpFile >> buf;
	 }
	 from->sendNotice("\002End of help.\002");
	 return;
      }
   }

   from->sendNotice(String("Couldn't find \002") +
		    command + "\002 in the helpfile.");
}

/* LastSeen - Find out when a user was seen last
 * Original 15/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::LastSeen(ServerConnection *cnx, Person *from,
			    String channel, String rest)
{
   String nick = from->getNick();
   
   // Make sure they are asking about someone
   if (!rest.length()) {
      from->sendNotice("I'm not a mind reader, who are you asking about?");
      return;
   }
   
   // Grab their nick
   StringTokens st(rest);
   String otherNick = st.nextToken();
   
   // Are they checking up on themselves?
   if (nick.toLower() == otherNick.toLower()) {
      from->sendNotice("If you want to look at yourself, buy a mirror");
      return;
   }
   
   // Are they looking at us?
   if (nick.toLower() == cnx->bot->nickName.toLower()) {
      from->sendNotice("But I'm right here, aren't I?");
      return;
   }
   
   // run through the userlist and find this guy
   for (list<UserListItem *>::iterator it = cnx->bot->userList->l.begin();
	it != cnx->bot->userList->l.end();
	it++) {
      if ((*it)->nick.toLower() == otherNick.toLower()) {
	 from->sendNotice(String("\002") + (*it)->nick + String("\002 ") +
			  (((*it)->lastseen <= 0) ? 
			   (((*it)->lastseen == 0) ?
			    String("is online now!") :
			    String("has no last seen information, sorry.")) :
			   (String("was last seen \002") +
			    Utils::timelenToStr(cnx->bot->currentTime.time -
						(*it)->lastseen) +
			    String("\002 ago."))));
	 
	 return;
      }
   }
   
   from->sendNotice(String("Sorry, I don't know when \002") + 
		    otherNick + String("\002 was on last."));
}

/* Note - Wrapper into the note interface
 */
void Commands::Note(ServerConnection *cnx, Person *from,
			String channel, String rest)
{
   from->sendNotice("The note interface is not operational.");
}

/* Ping - Ping a user back, tell them the time
 * Original 21/06/01, Pickle <pickle@alien.net.au>
 * Note: This only sends out the ping! The rest of the code is with the PING
 *       reply parser.
 */
void Commands::Ping(ServerConnection *cnx, Person *from,
			String channel, String rest)
{
   from->sendCTCP("PING",String(cnx->bot->currentTime.time) + 
		  String(":") + String(cnx->bot->currentTime.millitm));
}

/* Test - Test command to try out funky new stuff
 * Do not allow public access!! :)
 */
void Commands::Test(ServerConnection *cnx, Person *from,
			String channel, String rest)
{
//   from->sendNotice("\002Begin test dump.\002");
//   from->sendNotice(String("channel = ") + String(channel));
//   from->sendNotice(String("rest = \002#\002") + String(rest) +
//		    String("\002#\002"));
   
   // Split out the first word
   StringTokens st(rest);
   String word = st.nextToken();
   
   from->sendNotice(String("\002!\002") + word + String("\002!\002") +
		    Utils::generateSHA1(word) + String("\002!\002"));
}

/* Time - Tell someone the time (duh)
 * Original 6/7/01, Simon Butcher <simonb@alien.net.au>
 */
void Commands::Time(ServerConnection *cnx, Person *from,
			String channel, String rest)
{
   struct tm *timeNow = localtime(&cnx->bot->currentTime.time);
   
   from->sendNotice(String("The time is \002") +
		    String(timeNow->tm_hour).prepad(2, '0') + String(":") +
		    String(timeNow->tm_min).prepad(2, '0') + String(":") +
		    String(timeNow->tm_sec).prepad(2, '0') + String(".") +
		    String(cnx->bot->currentTime.millitm).prepad(3, '0') + 
		    String("\002, \002") + 
		    Utils::intToDayOfWeek(timeNow->tm_wday) + String(" ") + 
		    String(timeNow->tm_mday) + String(" ") +
		    Utils::intToMonth(timeNow->tm_mon) + String(" ") +
		    String(timeNow->tm_year + 1900) + String("\002."));
}

/* Stats - Show a list of statistical mumbo jumbo to impress stupid people
 * Original 19/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::Stats(ServerConnection *cnx, Person *from,
			 String channel, String rest)
{
   Version::sendInformation(cnx, from);

   if (rest.length() != 0) {
      from->sendNotice(String("Statistics for \002") + rest +
		       String("\002:"));
      from->sendNotice("This command needs to be finished.");
   } else {
      int num_chans, num_users, num_bots, num_suspend;
      int num_online, num_chanppl, num_chanops, num_chans_on;
      int num_lvluser, num_lvltrusted, num_lvlfriend, num_lvlmaster;
      int num_chanvoiced, num_nopass, peak_chanppl, peak_chanops;
      int peak_chanvoiced, num_dcc_con, num_chanbans, peak_chanbans;
      time_t uptime = cnx->bot->currentTime.time - cnx->bot->startTime;
      
      // This is a scarey way of zeroing every variable!
      num_chans = num_users = num_bots = num_suspend = 
	num_online = num_chanppl, num_chanops = num_chans_on =
	num_lvluser = num_lvltrusted = num_lvlfriend = num_lvlmaster = 
	num_chanvoiced = num_nopass = peak_chanppl = peak_chanops =
	peak_chanvoiced = num_dcc_con = num_chanbans = peak_chanbans = 0;
      
      for (list<UserListItem *>::iterator it = cnx->bot->userList->l.begin();
	   it != cnx->bot->userList->l.end();
	   it++) {
	 num_users++;
	 if ((*it)->flags & USERFLAG_IS_BOT)
	   num_bots++;
	 if ((*it)->flags & USERFLAG_IS_SUSPENDED)
	   num_suspend++;
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
			String("?: ").prepad(18) +
			String(0).pad(6) +
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
			String("?: ").prepad(18) +
			String(0).pad(6) +
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

