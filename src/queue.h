#ifndef __QUEUE_H_
#define __QUEUE_H_

#include "socket.h"

class Queue {
   Socket * sock;
#ifdef DEBUG
   bool debug;
#endif
   
 public:
#ifdef DEBUG
   Queue(Socket *, bool=false);
#else
   Queue(Socket *);
#endif
   virtual ~Queue();
   
   virtual bool sendLine(String);
};

#endif
