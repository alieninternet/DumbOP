
#include "utils.h"
#include "person.h"

Person::Person(Bot *b, String a)
  : bot(b), address(a)
{ }

Person::Person(Person &p)
  : bot(p.bot), address(p.address)
{ }

Person *
Person::copy()
{
  return new Person(*this);
}

String
Person::getNick() const
{
  return Utils::getNick(address);
}

String
Person::getAddress() const
{
  return address;
}

Person &
Person::operator=(const String & a)
{
  address = a;
  return *this;
}

/* sendNotice (virtual) - Quick way of sending notices
 * Original 13/12/00, Pickle <pickle@alien.net.au>
 */
void Person::sendNotice(String message)
{
   bot->serverConnection->queue->sendNotice(getNick(), message);
}

/* sendCTCP (virtual) - Quick way of sending CTCP replies
 * Original 30/12/00, Pickle <pickle@alien.net.au>
 */
void Person::sendCTCP(String command, String query)
{
   bot->serverConnection->queue->sendCTCP(getNick(), command, query);
}

/* sendCTCPReply (virtual) - Quick way of sending CTCP replies
 * Original 30/12/00, Pickle <pickle@alien.net.au>
 */
void Person::sendCTCPReply(String command, String query)
{
   bot->serverConnection->queue->sendCTCPReply(getNick(), command, query);
}
