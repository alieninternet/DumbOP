/* src/telnet/descriptor/session.cc
 * Telnet descriptor (connection) session input/output routines
 */

#include "config.h"

#include <ctype.h>

#include "telnet.h"
#include "telnetdialogue.h"
#include "protocol.h"
#include "socket.h"
#include "ansi.h"


/* write - Write a string to the socket
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


/* handleInput - Handle telnet server input
 * Original 02/01/01, Simon Butcher <simonb@alien.net.au>
 * Note: Yes! I know what you're thinking. He's a lazy sod who likes to chew
 * 	 up network bandwidth. YES! I realise that echoing a char back is not
 *       the nicest way to use the telnet protocol, shh. Yes, this should be
 *       fixed up properly.. Yes, I realise every char someone types is a tcp
 *       packet with one byte, plus a tcp packet back with one byte and that
 *       is several bytes just in that one letter typed. I cannot fix the char
 *       received (I know line mode would be handy) mainly because we then
 *       loose the advantages of controling the terminal strictly and also
 *       miss out on the ^L, which is handy not only for bugs but dodgey
 *       terminals.. SHHHH!
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
	  case TC_IAC:	// Telnet 'Interpret As Command' character.
	    break;
	  case ESC_CHAR:// ANSI code could be following....
	    break;
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


