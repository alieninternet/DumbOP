
#include "commands.h"
#include "flags.h"
#include "utils.h"

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

