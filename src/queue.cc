
#include <iostream.h>

#include "queue.h"

Queue::Queue(Socket * s, bool d)
  : sock(s), debug(d)
{ }

Queue::~Queue()
{ }

bool
Queue::sendLine(String s)
{
  if (debug)
    cout << "=> " << s << endl;
  return sock->write(s);
}
