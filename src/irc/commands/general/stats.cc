/* src/irc/commands/general/stats.cc
 * STATS command
 * Copyright (c) 1999-2001 Alien Internet Services
 */

#include "config.h"

#include <time.h>

#include "commands.h"
#include "flags.h"
#include "utils.h"
#include "user.h"

/* Stats - Show a list of statistical mumbo jumbo to impress stupid people
 * Original 19/12/00, Pickle <pickle@alien.net.au>
 * 31/07/01 simonb - Added 'user' stats section
 * Note: 68 chars should be our target 'max' chars per line, to fit in with
 * 	 'classic' ircII on an 80-char-wide terminal
 */
void Commands::Stats(Person *from, String channel, String rest)
{
   String destination = "";
   
   // Are we doing stats on this user or someone/thing else?
   if (!rest.length()) {
      // Make rest equal the from user nick
      destination = from->getNick();
   } else {
      // HACK. THIS NEEDS TO BE FINISHED!! Look for sub-commands etc
      destination = rest;
   }

   // We doing channel stats?
   if (Utils::isChannel(rest)) {
      // FINISH THIS DAMMIT!!!
      from->sendLine("I don't have any channel stats yet, sorry.");
      return;
   } else if (destination.toLower() == from->cnx->bot->nickName.toLower()) { 
      // Consider this to be asking info about me
      from->sendLine("I'm not that interesting, honestly.");
   } else { // Blatantly consider this to be a nickname!
      UserListItem *uli = 0;
      
      if (destination == from->getNick()) {
	 uli = from->cnx->bot->userList->getUserListItem(from->getAddress());
      } else {
	 uli = from->cnx->bot->userList->getUserListItemNick(destination);
      }
      
      // Make sure we got a user, or this will get ugly..
      if (uli) {
	 // Calculate some stats before we jump into the mess
	 struct tm *regoTime = localtime(&uli->registered);
	 
	 // Send the header
	 from->sendLine(String("\037") +
			  ((destination == from->getNick()) ?
			   String("Your statistics") : 
			   (String("Statistics for \002") +
			    uli->nick + String("\002"))) +
			  String(":\037"));
	 
	 // The stats - 66 char limit per line.
	 from->sendLine(String("Registered: ").prepad(12) + String("\002") +
			  ((uli->registered <= 0) ?
			   String("Unknown") :
			   (String(regoTime->tm_mday).prepad(2, '0') + 
			    String("/") +
			    String(regoTime->tm_mon).prepad(2, '0') + 
			    String("/") +
			    String(regoTime->tm_year + 
				   1900))).pad(10) + String("\002") +
			  String("Credits: ").prepad(12) + String("\002") +
			  (((uli->flags & USERFLAG_HAS_MONEY_TREE) ||
			    (uli->level >= User::MANAGER)) ?
			   String("Unlimited") :
			   ((uli->credits == 0) ? 
			    String("None") :
			    String(uli->credits))).pad(10) + String("\002") /*+
			  String("Uhh: ").prepad(12) +
			  String("Nothing..").pad(10)*/);
      } else {
	 from->sendLine(String("I'm sorry, I don't know who ") +
			  ((destination == from->getNick()) ?
			   String("you are") : (destination + String(" is"))) +
			  String("!"));
	 return;
      }
   }
   
   
   
   
   
   
   
/*   Version::sendInformation(from->cnx, from);
*
 *  if (rest.length() != 0) {
  *    from->sendLine(String("Statistics for \002") + rest +
*		       String("\002:"));
 *     from->sendLine("This command needs to be finished.");
  * } else {
   *   int num_chans, num_users, num_bots, num_suspend;
    *  int num_online, num_chanppl, num_chanops, num_chans_on;
     * int num_lvluser, num_lvltrusted, num_lvlfriend, num_lvlmaster;
*      int num_chanvoiced, num_nopass, peak_chanppl, peak_chanops;
 *     int peak_chanvoiced, num_dcc_con, num_chanbans, peak_chanbans;
  *    time_t uptime = from->cnx->bot->currentTime.time - from->cnx->bot->startTime;
   *   
    *  // This is a scarey way of zeroing every variable!
     * num_chans = num_users = num_bots = num_suspend = 
*	num_online = num_chanppl, num_chanops = num_chans_on =
*	num_lvluser = num_lvltrusted = num_lvlfriend = num_lvlmaster = 
*	num_chanvoiced = num_nopass = peak_chanppl = peak_chanops =
*	peak_chanvoiced = num_dcc_con = num_chanbans = peak_chanbans = 0;
 *     
  *    for (list<UserListItem *>::iterator it = from->cnx->bot->userList->l.begin();
*	   it != from->cnx->bot->userList->l.end();
*	   it++) {
*	 num_users++;
*	 if ((*it)->flags & USERFLAG_IS_BOT)
*	   num_bots++;
*	 if ((*it)->flags & USERFLAG_IS_SUSPENDED)
*	   num_suspend++;
*	 if ((*it)->lastseen == 0)
*	   num_online++;
*	 switch ((*it)->level) {
*	  case 1: num_lvluser++;
*	    break;
*	  case 2: num_lvltrusted++;
*	    break;
*	  case 3: num_lvlfriend++;
*	    break;
*	  case 4: num_lvlmaster++;
*	    break;
*	 }
*	 if ((*it)->passwd == "")
*	   num_nopass++;
 *     }
  *    
   *   for (map<String, Channel *, less<String> >::iterator it = from->cnx->bot->channelList->begin(); 
*	   it != from->cnx->bot->channelList->end(); ++it) {
*	 Channel *chan = from->cnx->bot->channelList->getChannel((*it).first);
*	 
*	 num_chans++;
*	 num_chanppl += chan->count;
*	 num_chanops += chan->countOp;
*	 num_chanvoiced += chan->countVoice;
*	 peak_chanppl += chan->count_peak;
*	 peak_chanops += chan->countOp_peak;
*	 peak_chanvoiced += chan->countVoice_peak;
*	 if (from->cnx->bot->channelList->getChannel((*it).first)->joined)
*	   num_chans_on++;
*	 for (vector<BanEntry *>::iterator itB = chan->channelBanlist.begin();
*	      itB != chan->channelBanlist.end(); ++itB)
*	   num_chanbans++;
* //	 peak_chanbans += chan->countBans_peak;
*      }
 *     
  *    for (list<DCCConnection *>::iterator it = from->cnx->bot->dccConnections.begin();
*	   it != from->cnx->bot->dccConnections.end(); ++it) {
*	 num_dcc_con++;
 *     }
  *    
   *   from->sendLine(String("\002\037General Statistics:\037\002"));
    *  from->sendLine(String("Name: ").prepad(10) +
*		       String(from->cnx->bot->nickName + " (" +
*			      from->cnx->bot->wantedNickName + ")").pad(23) +
*		       String("  Lag Check: ") +
*		       Utils::timeBigToStr(from->cnx->lag));
* #ifdef DEBUG
*      from->sendLine(String("Up time: ").prepad(10) +
*		       (!(from->cnx->bot->debug) ?
*			Utils::timelenToStr(uptime) :
*			(Utils::timelenToStr(uptime).pad(45) +
*			 String("\026 DEBUG MODE \026"))));
* #else
*      from->sendLine(String("Up time: ").prepad(10) +
*		       Utils::timelenToStr(uptime));
* #endif
*      from->sendLine(String("IRC data received: ").prepad(20) +
*		       (String((long)(from->cnx->receivedLen / 1024)) +
*			String("k")).pad(15) +
*		       String("IRC data sent: ").prepad(20) +
*		       (String((long)(from->cnx->sentLen / 1024)) +
*			String("k")));
*
*
 *      from->sendLine(String("\002\037Number of users:\037 ") + 
*			String(num_users) + String(" (") + 
*			String(num_online) + String(" currently online)\002"));
 *      from->sendLine(String("Number of Bots: ").prepad(18) +
*			String(num_bots).pad(6) +
*			String("?: ").prepad(18) +
*			String(0).pad(6) +
*			String("Suspended: ").prepad(18) +
*			String(num_suspend));
 *      from->sendLine(String("Normal Users: ").prepad(18) +
*			String(num_lvluser).pad(6) +
*			String("Trusted Users: ").prepad(18) +
*			String(num_lvltrusted).pad(6) +
*			String("Bot Friends: ").prepad(18) +
*			String(num_lvlfriend));
 *      from->sendLine(String("Bot Masters: ").prepad(18) +
*			String(num_lvlmaster).pad(6) +
*			String("?: ").prepad(18) +
*			String(0).pad(6) +
*			String("Users w/o passwd: ").prepad(18) +
*			String(num_nopass));
*
 *      from->sendLine(String("\002\037Number of channels:\037 ") + 
*			String(num_chans) + String(" (") +
*			String(num_chans_on) + 
*			String(" currently active)\002"));
 *      from->sendLine(String("Total People: ").prepad(16) +
*			(String(num_chanppl) + String(" (Peak: ") +
*			 String(peak_chanppl) + String(")")).pad(19) +
*			String("Total Banned: ").prepad(16) +
*			(String(num_chanbans) + String(" (Peak: ") +
*			 String(peak_chanbans) + String(")")));
 *      from->sendLine(String("Total Opped: ").prepad(16) +
*			(String(num_chanops) + String(" (Peak: ") +
*			 String(peak_chanops) + String(")")).pad(19) +
*			String("Total Voiced: ").prepad(16) +
*			(String(num_chanvoiced) + String(" (Peak: ") +
*			 String(peak_chanvoiced) + String(")")));
*
 *      from->sendLine(String("\002\037Number of DCC Fileserver Connections:\037 ") +
*			String(num_dcc_con) + String("\002"));
 *
 *
 *  }
 * 
*/
   
   
   
   
   
   // Send the user the last line..
   from->sendLine(String("\002End of statistics.\002"));
}

