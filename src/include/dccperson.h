
#ifndef __DCCPERSON_H_
#define __DCCPERSON_H_

#include "strings.h"
#include "person.h"
#include "dccconnection.h"

class DCCPerson : public Person {
  DCCConnection *dcc;
public:
  DCCPerson(DCCConnection *);
  DCCPerson(DCCPerson &);

  Person * copy();

  String getNick() const;
  String getAddress() const;

  void keepAlive();

  void sendNotice(String);
};

#endif
