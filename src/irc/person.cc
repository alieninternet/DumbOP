/* src/parser/person.cc
 * Person class for received data off IRC
 * Copyright (c) 1998-2001 Alien Internet Services
 */

#include "config.h"

#include "person.h"
#include "utils.h"
#include "flags.h"


Person::Person(String a, ServerConnection *c, UserListItem *u)
: address(a), 
  cnx(c), 
  uli(u)
{
}


Person::Person(Person &p)
: address(p.address), 
  cnx(p.cnx), 
  uli(p.uli)
{
}


Person * Person::copy()
{
   return new Person(*this);
}

String Person::getNick() const
{
   return Utils::getNick(address);
}

String Person::getAddress() const
{
   return address;
}

Person &Person::operator=(const String & a)
{
   address = a;
   return *this;
}

/* sendNotice (virtual) - Quick way of sending notices
 * Original 13/12/00, Pickle <pickle@alien.net.au>
 */
void Person::sendNotice(String message)
{
   cnx->queue->sendNotice(getNick(), message);
}


/* sendPrivmsg (virtual) - Quick way of sending privmsg's
 * Original 02/08/01, Simon Butcher <simonb@alien.net.au>
 */
void Person::sendPrivmsg(String message)
{
   cnx->queue->sendPrivmsg(getNick(), message);
}

/* sendCTCP (virtual) - Quick way of sending CTCP replies
 * Original 30/12/00, Pickle <pickle@alien.net.au>
 */
void Person::sendCTCP(String command, String query)
{
   cnx->queue->sendCTCP(getNick(), command, query);
}


/* sendCTCPReply (virtual) - Quick way of sending CTCP replies
 * Original 30/12/00, Pickle <pickle@alien.net.au>
 */
void Person::sendCTCPReply(String command, String query)
{
   cnx->queue->sendCTCPReply(getNick(), command, query);
}


/* sendLine - Send a line of test, either via a notice or privmsg
 * Original 02/08/01, Simon Butcher <simonb@alien.net.au>
 */
void Person::sendLine(String line)
{
   // Make sure the uli pointer is valid!
   if (uli) {
      // Find their preference
      if (uli->flags & USERFLAG_USE_PRIVMSG_ON_PRIV) {
	 sendPrivmsg(line);
      } else {
	 sendNotice(line);
      }
   } else {
      // No preference, send a notice
      sendNotice(line);
   }
}
