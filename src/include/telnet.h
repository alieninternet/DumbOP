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
# include "telnetdialogue.h"

# define TELNETFLAG_CONNECTED		0x00000001 // Connected
# define TELNETFLAG_HAS_ANSI		0x00000002 // Has ANSI capability?
# define TELNETFLAG_IN_TELNET_CODE	0x00000004 // Reading a telnet code?
# define TELNETFLAG_IN_ANSI_CODE	0x00000008 // Reading an ANSI code?
# define TELNETFLAG_TELETYPE		0x00000010 // Echoing printable chars?
# define TELNETFLAG_TELETYPE_MASK	0x00000020 // Masking inputted chars?
# define TELNETFLAG_TELETYPE_NO_BS	0x00000040 // Not allowing backspace?
# define TELNETFLAG_TELETYPE_CONTROL	0x00000080 // Allow control chars?

# define TELNET_NEWLINE			"\r\n"	// Full CRLF for compatibility
# define TELNET_BACKSPACE		"\b \b"	// Back, clear, back again.

class Telnet;

class TelnetDescriptor {
 private:
   void parseTelnet(String);			// Parse a telnet code
   void telnetWill(char);			// Send a telnet WILL
   void telnetWont(char);			// Send a telnet WONT
   void telnetDo(char);				// Send a telnet DO
   void telnetDont(char);			// Send a telnet DONT
   
 public:
   Telnet *telnet;				// Recursive back
   Socket *sock;				// Socket they are connected to
   UserListItem *user;				// Logged in user data
   
   time_t connectedTime;			// Time user logged on
   time_t lastActed;				// Time user last 'spoke'
   unsigned long flags;				// Connection flags

   unsigned short rows;				// Number of ROWS on tty
   unsigned short columns;			// Number of COLUMNS on tty

   String barMessage;				// Header bar message
   TelnetDialogue *dialogue;			// Current dialogue page
   TelnetDialogue *nextDialogue;		// Next dialogue
   
   TelnetDescriptor(Telnet *, Socket *);	// Constructor

   void handleInput(void);			// Deal with incoming char(s)
   void goodbyeSocket(void);			// Shutdown sockets
   void write(String);				// Write a string

   void headerInit(void);			// Initialise the header bar
   void headerUpdate(void);			// Update the header bar
   void headerMessage(String = "");		// Change the header message

   void bumpDialogue(void);			// Change the dialogue, maybe
   
   friend class TelnetDialogue;
};

class Socket;
class Bot;

class Telnet {
 private:
   int port;
   Socket *sock;
   
   list<class TelnetDescriptor *> descList;
   
   bool makeSocket();
   bool newConnection();
   
   void cleanDescs();

   time_t lastBarUpdate;		// Last time the status bar was updated
   
 public:
   Bot *bot; // Recursive

   Telnet(Bot *, int);
   ~Telnet();

   void attend(void);			// Called by main loop

   void monitorLine(String, String, 
		    String);		// Called by main IRC parsing routine
   
   // this below needs to be moved.
   static String IRCtoANSI(String, String, String, String);
   
   friend class TelnetDescriptor;
   
   friend class Socket;
   friend class Bot;
};

#endif
