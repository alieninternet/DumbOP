
#ifndef __SERVERCONNECTION_H_
# define __SERVERCONNECTION_H_

# include <time.h>
# include <sys/timeb.h>

# include "server.h"
# include "serverqueue.h"
# include "socket.h"

class Bot;
class Parser;
class Channel;
class Commands;

class ServerQueue;

class ServerConnection {
 protected:
   Socket *socket;				// Data socket
   
 private:
   Bot *bot; 					// Recursive back
   Server *server;				// Server details

   // Connection type
   enum {
      USER = 0,					// Normal user connection
      SERVICE = 1,				// Service connection
      SERVER_IRCD = 2,				// Vanilla IRCd server
      SERVER_AUSTNET = 3,			// Austnet style server
   } type;					
   
   unsigned long receivedLen;			// Bytes received
   unsigned long sentLen;			// Bytes send

   ServerQueue *queue;				// Data output queue
   
   struct timeb pingTime;			// Time at last ping
   long long lag;				// Millisecs of calculated lag

   time_t serverLastSpoken;			// Time the server last spoke
   
 public:
   ServerConnection(Bot *, Server *, String);	// Class constructor
   ~ServerConnection();				// Class destructor
   
   int getFileDescriptor() const;		// Get the socket descriptor

   bool connect();				// Connect to the server
   bool handleInput();				// Handle a line of input
   
   friend class Bot;
   friend class Person;
   friend class Parser;
   friend class Channel;
   friend class Commands;
   friend class CTCP;
   friend class Version;
   friend class ServerQueue;
   friend class Games;
   friend class GameQuiz;
};

#endif
