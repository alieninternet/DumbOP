/* telnet.cc
 * Handle telnet connections
 */

#include <list.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "telnet.h"
#include "socket.h"
#include "ansi.h"

/* Telnet - Initialise and startup the telnet daemon stuff
 * Original 2/1/01, Pickle <pickle@alien.net.au>
 */
Telnet::Telnet(Bot *b, int portNum)
 : port(portNum), 
 sock(0), 
 bot(b),
 lastBarUpdate(b->currentTime.time)
{
   if (makeSocket()) {
      // umm..
   } 
}

/* ~Telnet - Shutdown the telnet daemon stuff
 * Original 2/1/01, Pickle <pickle@alien.net.au>
 */
Telnet::~Telnet()
{
#ifdef DEBUG
   if (bot->debug)
     cout << "Closing down open telnet connections" << endl;
#endif
   
   // Close down all active sockets
   telnetDescriptor *d;
   while (descList.size() != 0) {
      d = *descList.begin();

      if (d->sock->isConnected()) {
	 if (d->flags & TELNETFLAG_CONNECTED) {
	    d->goodbyeSocket();
	 } else
	   d->sock->close();
      }
      
      descList.erase(descList.begin());
      delete d->sock;
      delete d;
   }
   
   for (list<telnetDescriptor *>::iterator it = descList.begin();
	it != descList.end(); it++)
     if ((*it)->sock->isConnected()) {
	//	if ((*it)->flags & TELNETFLAG_CONNECTED)
	// Say goodbye?
	(*it)->sock->close();
     }
   
#ifdef DEBUG
   if (bot->debug)
     cout << "Closing down main telnet socket" << endl;
#endif
   
   sock->close();
   delete sock;
}

/* handleInput - Handle telnet server input
 * Original 2/1/01, Pickle <pickle@alien.net.au>
 */
void telnetDescriptor::handleInput()
{
}
  
/* goodbyeSocket - Say goodbye and close the socket down
 * Original 2/1/01, Pickle <pickle@alien.net.au>
 */
void telnetDescriptor::goodbyeSocket()
{
   flags = 0; // Should actually be not connected, leave other flags intact
   sock->close();
}

/* write
 * Original 2/1/01, Pickle <pickle@alien.net.au>
 */
void telnetDescriptor::write(String s)
{
   if ((flags & TELNETFLAG_CONNECTED) && 
       (sock->isConnected()))
     if (!sock->write(s)) {
	sock->close();
	flags = 0; // Should be plus ~TELNETFLAG_CONNECTED, leave rest
     }
}

/* makeSocket - Create a socket for the telnet server
 * Original 2/1/01, Pickle <pickle@alien.net.au>
 */
bool Telnet::makeSocket()
{
   int options;

   sock = new Socket();
   
   if (!sock) {
      // Failed to create the socket - Should log here
      return false;
   }
   
   options = 1;

   if (setsockopt(sock->getFileDescriptor(), SOL_SOCKET, SO_REUSEADDR, 
		  (char *)&options, sizeof(options)) < 0) {
      // Couldn't set socket options - Should log here
      // Non fatal.
   }

#ifdef DEBUG
   if (bot->debug)
     cout << "Binding telnet to port..." << endl;
#endif
   
   if (!sock->bindPort(port)) {
      // Failed to bind socket to port - Should log here
      sock->close();
      return false;
   }

   sock->listen();

   return true;
}

/* newConnection - Process a new connection
 * Original 2/1/01, Pickle <pickle@alien.net.au>
 */
bool Telnet::newConnection()
{
   Socket *newsock;
   time_t currentTime = time(NULL);
   
#ifdef DEBUG
   if (bot->debug)
     cout << "New telnet connection..." << endl;
#endif
   
   newsock = new Socket(sock->accept());
   
   if (!newsock || !newsock->isConnected())
     return false;
   
   // Host name check for acceptance should be done here
   // Slave lookup should be done here
  
#ifdef DEBUG
   if (bot->debug)
     cout << "Acception telnet connection! Adding to descriptor list.." << endl;
#endif

   /* TEMPORARY */   
   newsock->write(ANSI::telnetHeaderInit());
   newsock->write(TelnetSpy::spyHeaderInit());
   /* TEMPORARY */
   
   descList.push_back(new telnetDescriptor(newsock,
					   currentTime, currentTime,
//					   TELNETFLAG_CONNECTED,
					   0xFFFF, // TEMPORARY.. Honest! :)
					   String("")));

   return true;
}

/* cleanDescs - Clean out disconnected descriptors
 * Original 2/1/01, Pickle <pickle@alien.net.au>
 */
void Telnet::cleanDescs()
{
//   for (list<telnetDescriptor *>::iterator it = descList.begin();
//	it != descList.end(); it++)
}

/* attend - Called from main loop to update telnet consoles
 * Original 8/7/01, Simon Butcher <simonb@alien.net.au>
 */
void Telnet::attend(void)
{
   bool updateBars = false;
   
   // Time to update the status bar? We do this once a minute
   if (bot->currentTime.time >= (lastBarUpdate + 60)) {
      lastBarUpdate = bot->currentTime.time;
      updateBars = true;
   }
   
   // Run through the descriptor list to see what needs doing
   for (list<telnetDescriptor *>::iterator it = descList.begin();
	it != descList.end(); it++) {
      // Is this descriptor connected?
      if ((*it)->flags & TELNETFLAG_CONNECTED) {
	 // Are we set to update the bars this time around?
	 if (updateBars) {
	    (*it)->write(ANSI::telnetHeaderUpdate());
	 }
      }
   }
}
