
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

void
Person::sendNotice(String message)
{
  bot->serverConnection->queue->sendNotice(getNick(), message);
}
