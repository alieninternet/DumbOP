/* telnet.h
 * Handle telnet connections
 */

#ifndef __TELNET_H_
# define __TELNET_H_

# include <time.h>
# include <list.h>

# include "strings.h"
# include "socket.h"
# include "bot.h" 
# include "telnetspy.h"

# define TELNETFLAG_CONNECTED		0x00000001 // Connected
# define TELNETFLAG_HAS_ANSI		0x00000002 // Has ANSI capability?
# define TELNETFLAG_TELETYPE		0x00000004 // Echoing printable chars?
# define TELNETFLAG_TELETYPE_MASK	0x00000008 // Masking inputted chars?
# define TELNETFLAG_IN_TELNET_CODE	0x00000010 // Reading a telnet code?
# define TELNETFLAG_IN_ANSI_CODE	0x00000020 // Reading an ANSI code?

# define TELNET_TELETYPE_MASK_CHAR	'*'	// Char to mask input with
# define TELNET_DEFAULT_BAR_MESSAGE	"Management Console" // Bar message
# define TELNET_ASSUMED_TTY_ROWS	24
# define TELNET_ASSUMED_TTY_COLUMNS	80


class Telnet;

class telnetDescriptor {
   enum {
      PAGE_LOGIN = 0,				// Login window
      PAGE_MAIN_MENU,				// Main menu
      PAGE_SPY					// Monitoring
   };
   
 public:
   Telnet *telnet;				// Recursive back
   Socket *sock;				// Socket they are connected to
   time_t connectedTime;			// Time user logged on
   time_t lastActed;				// Time user last 'spoke'
   unsigned long flags;				// Connection flags

   unsigned short rows;				// Number of ROWS on tty
   unsigned short columns;			// Number of COLUMNS on tty

   String barMessage;				// Header bar message
   unsigned char page;				// Current page
   
   telnetDescriptor(Telnet *, Socket *);	// Constructor

   void handleInput();				// Deal with incoming char(s)
   void goodbyeSocket();			// Shutdown sockets
   void write(String);				// Write a string

   void headerInit();				// Initialise the header bar
   void headerUpdate();				// Update the header bar
   void headerMessage(String = "");		// Change the header message

   friend class TelnetSpy;
};

class Socket;
class Bot;

class Telnet {
 private:
   int port;
   Socket *sock;
   Bot *bot; // Recursive
   
   list<class telnetDescriptor *> descList;
   
   bool makeSocket();
   bool newConnection();
   
   void cleanDescs();

   time_t lastBarUpdate;		// Last time the status bar was updated

   friend class TelnetSpy;
   
 public:
   Telnet(Bot *, int);
   ~Telnet();

   void attend(void);			// Called by main loop
   
   friend class telnetDescriptor;
   
   friend class Socket;
   friend class Bot;
};

#endif
