#ifndef __QUEUE_H_
#define __QUEUE_H_

#include "socket.h"

class Queue {
  Socket * sock;
  bool debug;
public:
  Queue(Socket *, bool=false);
  virtual ~Queue();
  
  virtual bool sendLine(String);
};

#endif
