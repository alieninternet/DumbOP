/* telnet/descriptor.cc
 * Telnet descriptor (connection) routines
 */

#include "config.h"

#include <list.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ctype.h>

#include "telnet.h"
#include "telnetdialogue.h"
#include "socket.h"
#include "ansi.h"
#include "utils.h"


/* TelnetDescriptor - Descriptor constructor
 * Original 02/01/01, Simon Butcher <simonb@alien.net.au>
 */
TelnetDescriptor::TelnetDescriptor(Telnet *t, Socket *s)
: telnet(t), 
  sock(s),
  user(0),
  connectedTime(telnet->bot->currentTime.time),
  lastActed(connectedTime),
  flags(TELNETFLAG_CONNECTED),
  rows(TELNET_ASSUMED_TTY_ROWS),
  columns(TELNET_ASSUMED_TTY_COLUMNS), 
  barMessage(TELNET_DEFAULT_BAR_MESSAGE),
  dialogue(new TelnetDialogueLogin(this)),
  nextDialogue(0)
{
   /* TEMPORARY */
   flags |= TELNETFLAG_HAS_ANSI;
   headerInit();
   /* TEMPORARY */
   
   dialogue->drawPage();
}


/* handleInput - Handle telnet server input
 * Original 02/01/01, Simon Butcher <simonb@alien.net.au>
 */
void TelnetDescriptor::handleInput(void)
{
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
	 /* Check if we have this socket in teletype mode and it is not a
	  * newline char
	  */
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
	 
	 // The dialogue routines may need this char too
	 dialogue->parseInput(chr);
      } else { // A special char perhaps?
	 switch (chr) {
	  case '\b':	// BS - Backspace
	  case '\177':	// DEL - Delete (we treat it the same)
	    // Check if we are allowed to 'do' a backspace
	    if ((flags & TELNETFLAG_TELETYPE) && 
		!(flags & TELNETFLAG_TELETYPE_NO_BS)) {
	       sock->write(String(TELNET_BACKSPACE));
	    }
	    
	    // The dialogue routines may need this char too
	    dialogue->parseInput('\b');
	    bumpDialogue();
	    break;
	  case '\f':	// FF - Form Feed (or Page Eject/Redraw Screen)
	    /* Reinitialise the header. This will also clear the screen, 
	     * draw the current header bar message, and also write the time
	     */
	    headerInit();
	    
	    // Draw the page
	    dialogue->drawPage();
	    bumpDialogue();
	    break;
	  case '\n':	// CR - Carrage return
	  case '\025':	// NAK - Negative Acknowledge (Modern use: clear line)
	    // The dialogue routines may need this char
	    dialogue->parseInput(chr);
	    bumpDialogue();
	    break;
	  case '\021':	// DC1 - Device Control 1 (was XON, paper reader on)
	    // start output.
	    break;
	  case '\022':	// DC3 - Device Control 3 (was XOFF, paper reader off)
	    // stop output.
	    break;
	 }
      }
   }
}


/* goodbyeSocket - Say goodbye and close the socket down
 * Original 02/01/01, Simon Butcher <simonb@alien.net.au>
 */
void TelnetDescriptor::goodbyeSocket(void)
{
   flags |= ~TELNETFLAG_CONNECTED;
   sock->close();
}


/* write
 * Original 02/01/01, Simon Butcher <simonb@alien.net.au>
 */
void TelnetDescriptor::write(String s)
{
   if ((flags & TELNETFLAG_CONNECTED) && 
       (sock->isConnected())) {
      if (!sock->write(s)) {
	 sock->close();
	 flags |= ~TELNETFLAG_CONNECTED;
      }
   }
}


/* headerInit - Initialise header for telnet sessions
 * Original 30/12/00, Simon Butcher <simonb@alien.net.au>
 * Note: Clears the screen, draws the basic header, and flows through to
 *       further header routines to draw the message and time.
 */
void TelnetDescriptor::headerInit(void)
{
   write(String(ANSI_CLR_SCREEN) + String(ANSI_CUR_HOME) + 
	 String(ANSI_FINVERSE) + String(ANSI_CLR_LINE) + String(" ") + 
	 Version::getVersion().pad(columns - 1) + ANSI::scrollRegion(2, rows));
   
   /* Fire up the status bar with a new message. This will also call the
    * first header bar update to occur (eg. put the time etc on there)
    */
   headerMessage();
}


/* headerUpdate - Update header for telnet sessions
 * Original 30/12/00, Simon Butcher <simonb@alien.net.au>
 * Note: We save the cursor location before updating the bar. If the client
 * 	 does not support this ANSI string, TO HELL WITH THEM! Pretty bad
 * 	 client if it doesn't even support THAT.
 */
void TelnetDescriptor::headerUpdate(void)
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
 * Original 30/12/00, Simon Butcher <simonb@alien.net.au>
 * Note: We also prevoke a header update, since we clear the line.
 */
void TelnetDescriptor::headerMessage(String message = "")
{
   // Are we changing the message?
   if (message.length()) {
      barMessage = message;
   }

   // Pump out the message.
   write(String(ANSI_CUR_SAVE) + String(ANSI_FINVERSE) + 
	 ANSI::gotoXY(15, 1) + String(ANSI_CLR_EOL) +
	 ANSI::gotoXY((int)((columns / 2) - (barMessage.length() / 2)), 1) +
	 barMessage + String(ANSI_CUR_RESTORE) + String(ANSI_NORMAL));
   
   // Fire up the data we killed
   headerUpdate();
}


/* bumpDialogue - Change the dialogue to the next one, if required
 * Original 01/08/01, Simon Butcher <simonb@alien.net.au>
 */
void TelnetDescriptor::bumpDialogue(void)
{
   // Check if there is another dialogue to bump in
   if (nextDialogue) {
      // Bump out the old one, if it is safe
      if (dialogue) {
	 delete dialogue;
      }
      
      // Bump in the new one, and reset the nextDialogue variable
      dialogue = nextDialogue;
      nextDialogue = 0;
      
      // Draw the new dialogue page
      dialogue->drawPage();
   }
}
