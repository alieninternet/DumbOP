
#include "config.h"
#include "dccperson.h"

#include "serverconnection.h"
#include "userlistitem.h"

DCCPerson::DCCPerson(DCCConnection *cnx)
  : Person(cnx->nuh, (ServerConnection *)0, (UserListItem *)0), dcc(cnx)
{ }

DCCPerson::DCCPerson(DCCPerson &p)
  : Person(p), dcc(p.dcc)
{ }

void
DCCPerson::sendNotice(String message)
{
  dcc->sendNotice(message);
}

Person *
DCCPerson::copy()
{
  return (Person *)(new DCCPerson(*this));
}

void
DCCPerson::keepAlive()
{
  dcc->autoRemove = false;
}
