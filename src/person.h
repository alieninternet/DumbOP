
#ifndef __PERSON_H_
#define __PERSON_H_

#include "string.h"

class Bot;

class Person {
protected:
  Bot *bot;
  String address;
public:
  Person(Bot *, String = "");
  Person(Person &);

  virtual ~Person() { }

  virtual Person * copy();

  String getNick() const;
  String getAddress() const;

  virtual void keepAlive() { }

  virtual void sendNotice(String);
  virtual void sendCTCP(String, String);
  virtual void sendCTCPReply(String, String);
   
  Person & operator=(const String &);
};

#endif
