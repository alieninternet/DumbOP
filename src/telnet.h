/* telnet.h
 * Handle telnet connections
 */

#ifndef __TELNET_H_
# define __TELNET_H_

# include <time.h>
# include <list.h>

# include "string.h"
# include "socket.h"
# include "bot.h" 
# include "telnetspy.h"

# define TELNETFLAG_CONNECTED		0x0001	// Connected
# define TELNETFLAG_AUTHENTICATED	0x0002	// Logged on
# define TELNETFLAG_ANSI		0x0008	// Can support ANSI
# define TELNETFLAG_SPYING		0x0010	// Not in menu, spying
# define TELNETFLAG_CLI			0x0020	// Command-Line mode on

class telnetDescriptor {
 public:
   Socket *sock;
   time_t connectedTime;
   time_t lastActed;
   int flags;
   String spyChannel;
   
   telnetDescriptor(Socket *s, time_t ct, time_t la, int f, String sc)
     : sock(s), connectedTime(ct), lastActed(la), flags(f), 
       spyChannel(sc)
       {
       }

   void handleInput();
   void goodbyeSocket();
   void write(String);
};

class Socket;
class Bot;

class Telnet {
   int port;
   Socket *sock;
   Bot *bot; // Recursive
   
   list<class telnetDescriptor *> descList;
   
   bool makeSocket();
   bool newConnection();
   
   void cleanDescs();

   friend class TelnetSpy;
   
 public:
   Telnet(Bot *, int);
   ~Telnet();
   
   friend class Socket;
   friend class Bot;
};

#endif
