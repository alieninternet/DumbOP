/* telnet/descriptor.cc
 * Telnet descriptor (connection) routines
 */

#include <list.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ctype.h>

#include "telnet.h"
#include "socket.h"
#include "ansi.h"


/* telnetDescriptor - Descriptor constructor
 * Original 2/1/01, Pickle <pickle@alien.net.au>
 */
telnetDescriptor::telnetDescriptor(Telnet *t, Socket *s)
: telnet(t), 
  sock(s),
  connectedTime(telnet->bot->currentTime.time),
  lastActed(connectedTime),
  flags(TELNETFLAG_CONNECTED),
  rows(TELNET_ASSUMED_TTY_ROWS),
  columns(TELNET_ASSUMED_TTY_COLUMNS), 
  barMessage(TELNET_DEFAULT_BAR_MESSAGE),
  page(PAGE_LOGIN)
{

   page = PAGE_SPY;
   
   /* TEMPORARY */
   flags |= TELNETFLAG_HAS_ANSI;
   headerInit();
   /* TEMPORARY */
   flags |= TELNETFLAG_TELETYPE | TELNETFLAG_IN_ANSI_CODE;
   write(TelnetSpy::spyHeaderInit());
   /* TEMPORARY */
}


/* handleInput - Handle telnet server input
 * Original 2/1/01, Pickle <pickle@alien.net.au>
 */
void telnetDescriptor::handleInput()
{
//   String line = sock->readLine();
//   
//   if (line.length()) {
//      telnet->bot->logLine(String("telnet input: %") + line + String("%"));
//   }

   // Glue the 'last spoke' to the time now
   lastActed = telnet->bot->currentTime.time;
   
   // Read a char from the socket
   char chr = sock->readChar()[0];
   
   // Are we reading a telnet-protocol code, ANSI code, or running normally?
   if (flags & TELNETFLAG_IN_TELNET_CODE) { // NOTE: telnet comes first...
      
      // Turn off the telnet code flag thing.
      flags &= ~TELNETFLAG_IN_TELNET_CODE;
   } else if (flags & TELNETFLAG_IN_ANSI_CODE) { // NOTE: ANSI comes second...
      // Firstly, if this is an letter, it signifies the end of the ANSI code.
      if (isalpha(chr)) {
	 flags &= ~TELNETFLAG_IN_ANSI_CODE;
      }
      
      // Stuff should go here.
      
   } else {
      // Check if the char is printable
      if ((chr > 31) && (chr != 127)) {
	 // Check if we have this socket in teletype mode
	 if (flags & TELNETFLAG_TELETYPE) {
	    // Are we going to mask this output or not?
	    if (flags & TELNETFLAG_TELETYPE_MASK) {
	       // Output the mask char
	       sock->write(String(TELNET_TELETYPE_MASK_CHAR));
	    } else {
	       // Output the real char
	       sock->write(String(chr));
	    }
	 }
      } else { // A special char perhaps?
	 switch (chr) {
	  case '\b': // A backspace.
	    if (flags & TELNETFLAG_TELETYPE) {
	       /* Send a backspace to move back, a space to clear whatever char
		* we are clearing, and another backspace to jump back again.
		* We do this so verbosely because backspace is only to move
		* back one space, it doesn't normally include clearing the 
		* char -- that's our job as the terminal controller.
		*/
	       sock->write(String("\b \b"));
	    }
	    break;
	 }
      }
   }
}


/* goodbyeSocket - Say goodbye and close the socket down
 * Original 2/1/01, Pickle <pickle@alien.net.au>
 */
void telnetDescriptor::goodbyeSocket()
{
   flags |= ~TELNETFLAG_CONNECTED;
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
	flags |= ~TELNETFLAG_CONNECTED;
     }
}


/* headerInit - Initialise header for telnet sessions
 * Original 30/12/00, Pickle <pickle@alien.net.au>
 */
void telnetDescriptor::headerInit()
{
   write(String(ANSI_CUR_HOME) + String(ANSI_FINVERSE) + 
	 String(ANSI_CLR_LINE) + String(" ") + 
	 String(VERSION_STRING).pad(columns - 1) + 
	 ANSI::scrollRegion(2, rows));
   
   /* Fire up the status bar with a new message. This will also call the
    * first header bar update to occur (eg. put the time etc on there)
    */
   headerMessage();
}


/* headerUpdate - Update header for telnet sessions
 * Original 30/12/00, Pickle <pickle@alien.net.au>
 * Note: We save the cursor location before updating the bar. If the client
 * 	 does not support this ANSI string, TO HELL WITH THEM! Pretty bad
 * 	 client if it doesn't even support THAT.
 */
void telnetDescriptor::headerUpdate()
{
   struct tm *timeNow = localtime(&telnet->bot->currentTime.time);
   String timeStr = (Utils::intToMonth(timeNow->tm_mon) + String(" ") +
		     String(timeNow->tm_mday) + String(" ") +
		     String(timeNow->tm_year + 1900) + String(", ") +
		     String(timeNow->tm_hour).prepad(2, '0') + String(":") +
		     String(timeNow->tm_min).prepad(2, '0'));
   
   
   write(String(ANSI_CUR_SAVE) + String(ANSI_FINVERSE) + 
	 ANSI::gotoXY((columns - timeStr.length()), 1) + timeStr +
	 String(ANSI_CUR_RESTORE) + String(ANSI_NORMAL));
}


/* headerMessage - Update header message
 * Original 30/12/00, Pickle <pickle@alien.net.au>
 * Note: We also prevoke a header update, since we clear the line.
 */
void telnetDescriptor::headerMessage(String message = "")
{
   // Are we changing the message?
   if (message.length()) {
      barMessage = message;
   }

   // Pump out the message.
   write(String(ANSI_CUR_SAVE) + String(ANSI_FINVERSE) + String(ANSI_BOLD) +
	 ANSI::gotoXY(15, 1) + String(ANSI_CLR_EOL) + 
	 ANSI::gotoXY((int)((columns / 2) - (barMessage.length() / 2)), 1) +
	 barMessage + String(ANSI_CUR_RESTORE) + String(ANSI_NORMAL));
   
   // Fire up the data we killed
   headerUpdate();
}

