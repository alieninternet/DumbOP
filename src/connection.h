
#ifndef __CONNECTION_H_
#define __CONNECTION_H_

#include "string.h"
#include "socket.h"

class Connection {
 
public:
  Connection(String, int, String);
  Connection(unsigned long, int);
  
  virtual ~Connection();

  int getFileDescriptor() const;
  
  virtual bool connect() = 0;
  virtual bool handleInput() = 0;

protected:
  Socket socket;
};

#endif
