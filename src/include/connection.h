/* src/connection/class.cc
 * IRC Server connection routines
 * Copyright (c) 1995-2001 Alien Internet Services
 */

#ifndef __CONNECTION_H_
# define __CONNECTION_H_

# include "strings.h"
# include "socket.h"

class Connection {
 protected:
   Socket socket;
   
 public:
   Connection(String, int, String);
   Connection(unsigned long, int);
   
   virtual ~Connection();
   
   int getFileDescriptor() const;
   
   virtual bool connect() = 0;
   virtual bool handleInput() = 0;
   
};

#endif
