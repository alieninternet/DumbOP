
#include <fstream.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ctype.h>
#ifndef _X_OPEN_SOURCE
#define _X_OPEN_SOURCE
#endif
#ifndef _X_OPEN_SOURCE_EXTENDED
#define _X_OPEN_SOURCE_EXTENDED 1
#endif
#ifndef __USE_XOPEN
#define __USE_XOPEN
#endif
#include <unistd.h>
#include <time.h>

#include "commands.h"
#include "usercommands.h"
#include "flags.h"
#include "utils.h"

/* Access
 * Original 20/12/00, Pickle <pickle@alien.net.au>
 */
void UserCommands::Access(ServerConnection *cnx, Person *from,
			  String channel, String rest)
{
   String nick = from->getNick();
   
   if (!rest.length()) {
      User * u = cnx->bot->channelList->getChannel(channel)->getUser(nick);
      UserListItem * uli;
      
      if (u)
	uli = u->userListItem;
      else
	uli = cnx->bot->userList->getUserListItem(from->getAddress(),
						  channel);
      
      if (uli) {
	 from->sendNotice(String("You are \002") +
			  uli->mask.getMask() + "\002 on \002" +
			  uli->channelMask.getMask() + "\002");
	 from->sendNotice(String("Lvl: ") +
			  Utils::levelToStr(uli->level) +
			  " Prot: " +
			  Utils::protToStr(uli->prot) +
			  " Ident: " +
			  (uli && !uli->identified ? "\026" : "") +
			  Utils::boolToStr(uli && uli->identified) +
			  (uli && !uli->identified ? "\026" : ""));
      } else
	from->sendNotice(String("You are not in the userlist for \002") +
			 channel + "\002");
   } else {
      StringTokens st(rest);
      String otherNick = st.nextToken();
      
      if (otherNick == "-") {
	 int num = 0, num_on = 0;
	 
	 from->sendNotice(String("Access list search for mask \002") +
			  otherNick + String("\002 on \002") +
			  channel + String("\002:"));
	 from->sendNotice("\026 \037Nickname       \037 \037Level       \037 \037A+o\037 \037A+v\037 \037Right\037 \037Ident\037 \037Flags     \037 \026");
	 
	 for (list<UserListItem *>::iterator it = cnx->bot->userList->l.begin();
	      it != cnx->bot->userList->l.end();
	      it++) {
	    if (((*it)->channelMask.getMask().toLower() == channel.toLower()) &&
		!((*it)->flags & USERFLAG_IS_SUSPENDED)) {
	       from->sendNotice((((*it)->lastseen == 0) ?
				 (((*it)->flags & PERSONFLAG_IDENTIFIED) ?
				  String("\002*\002") :
				  String("\002+\002")) : 
				 String(" ")) +
				Utils::getFirstNick((*it)->nick).prepad(15) + " " +
				Utils::levelToStr((*it)->level).pad(12) + " " +
				(((*it)->flags & USERFLAG_IS_BOT) ?
				 String("Bot") : String("")) +
				Utils::boolToStr((*it)->flags & PERSONFLAG_IDENTIFIED).pad(5) + " " +
				Utils::flagsToStr((*it)->flags).pad(10));
	       num++;
	       if ((*it)->lastseen == 0)
		 num_on++;
	    }
	 }
	 
	 if (num == 0) 
	   from ->sendNotice("\002End of access list\002 (No matches)");
	 else 
	   from->sendNotice(String("\002End of access list\002 (") +
			    String((int)num) + " entries, " + 
			    String((int)num_on) + " online)");
      } else {
	 User * u = cnx->bot->channelList->getChannel(channel)->getUser(otherNick);
	 UserListItem * uli;
	 
	 if (u)
	   uli = u->userListItem;
	 else
	   uli = cnx->bot->userList->getUserListItem(otherNick + "!" +
						     cnx->bot->getUserhost(channel,
									   otherNick),
						     channel);
	 
	 if (uli) {
	    from->sendNotice(String("\002") + otherNick +
			     String("\002 is \002") + uli->mask.getMask() +
			     String("\002 on \002") + uli->channelMask.getMask() + 
			     String("\002"));
	    from->sendNotice(String("Lvl: ") +
			     Utils::levelToStr(uli->level) +
			     " Prot: " +
			     Utils::protToStr(uli->prot) +
			     " Ident: " +
			     Utils::boolToStr(uli && uli->identified));
	 } else {
	    from->sendNotice(String("\002") + otherNick +
			     String("\002 is not in the userlist for \002") +
			     channel + String("\002"));
	 }
      }
   }
}

/* Ident
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 */
void UserCommands::Identify(ServerConnection *cnx, Person *from,
			 String channel, String rest)
{
   // Is this secure enough to continue?
   if (Utils::isChannel(channel)) {
      from->sendNotice("For security reasons you should not use that command on a channel.");
      return;
   }

   // Split out the first word
   StringTokens st(rest);
   String word = st.nextToken();
   
   // Make sure we got something
   if (!word.length()) {
      from->sendNotice("You must supply a password to identify yourself!");
      return;
   }
   
   /* Check if we can identify this use properly. Note in requesting the
    * boolean of identification, it also marks this user AS identified as
    * well..
    */
   if (cnx->bot->userList->identify(from->getNick(), word)) {
      // Tell the user they have been identified correctly
      from->sendNotice(String("You have been identified as \002") + 
		       from->getNick() + String("\002."));
   } else {
      // Tell the user they were not identified
      from->sendNotice("Nickname/Password mismatch - You have not been identified.");
   }
}

/* NsLookup - Do a name server lookup
 * Original 15/12/00, Pickle <pickle@alien.net.au>
 * Needs: Reverse lookup capabilities and to be ported in having nslookups
 *        via a forked slave portion of the code like TG did
 */
void UserCommands::NsLookup(ServerConnection *cnx, Person *from,
			    String channel, String rest)
{
   String target;
   
   if (rest.length() == 0) {
      from->sendNotice("\002You need to supply a nick or a "
		       "host for this command.\002");
      return;
   }
   
   if (rest.find('.') == -1) {
      StringTokens st(cnx->bot->getUserhost("", rest));
      st.nextToken('@');
      target = st.rest();
      if (target.length() == 0) {
	 from->sendNotice(String("\002I could not find\002 ") +
			  target);
	 return;
      }
   } else
     target = rest;
   
   struct hostent * host;
   struct in_addr iaddr;
   
   if (isdigit(target[0])) { // An IP ?
      iaddr.s_addr = inet_addr((const char *)target);
      host = gethostbyaddr((char *)(&iaddr),
			   sizeof(struct in_addr),
			   AF_INET);
      if (host) {
	 from->sendNotice(target +
			  " \002is the IP address of\002 " +
			  host->h_name);
	 return;
      }
   } else {
      host = gethostbyname((const char *)target);
      if (host) {
	 memcpy((char *)&iaddr, (char *)host->h_addr,
		host->h_length);
	 from->sendNotice(target + " \002has\002 " +
			  inet_ntoa(iaddr) + " \002for IP address.\002");
	 return;
      }
   }
   
   from->sendNotice(String("\002I could not find host\002 ") + target);
}

/* Password - change a password
 * Original 16/12/00, Pickle <pickle@alien.net.au>
 * Needs: Fixing.
 */
void
UserCommands::Password(ServerConnection *cnx, Person *from,
                       String channel, String rest)
{
   // Is this secure enough to continue?
   if (Utils::isChannel(channel)) {
      from->sendNotice("For security reasons you should not use that command on a channel.");
      return;
   }
}

/* Register - Add a user, or channel, at somebody's request
 * Original 19/07/01, Simon Butcher <simonb@alien.net.au>
 */
void UserCommands::Register(ServerConnection *cnx, Person *from,
			    String channel, String rest)
{
   // Is this secure enough to continue?
   if (Utils::isChannel(channel)) {
      from->sendNotice("For security reasons you should not use that command on a channel.");
      return;
   }
   
   // Split out the first word
   StringTokens st(rest);
   String word = st.nextToken();
   
   // Make sure we got something
   if (!word.length()) {
      from->sendNotice("You must supply a password!");
      return;
   }
   
   // Are we registering a channel or not?
   if (Utils::isChannel(word)) {
      from->sendNotice(String("You cannot register a channel with this command."));
   } else {
      // Make sure we do not have a duplicate!
      if (cnx->bot->userList->isNickInUserList(from->getNick())) {
	 from->sendNotice(String("Your nickname is already registered!"));
	 return;
      }
      
      // Create the mask. Notice how unrestricted this mask really is...
      String mask = from->getNick() + String("!*@*");

      // Tell the user we are adding them, and add them
      from->sendNotice(String("Registering you as \002") + mask +
		       String("\002 with the password specified."));
      cnx->bot->userList->addUser(mask, "#*", User::USER, 0, from->getNick(),
				  0, 0, Utils::generateSHA1(word));
      
      // They have registered, technically they have also identified too.
      from->flags &= PERSONFLAG_IDENTIFIED;
   }
}

