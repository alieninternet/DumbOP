/* src/telnet/descriptor/class.cc
 * Telnet descriptor (connection) routines
 */

#include "config.h"

#include <list.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ctype.h>

#include "telnet.h"
#include "protocol.h"
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

   /* Tell the telnet client a thing or too... This should be considered
    * our initial terminal setup, the remote client might dispute it but
    * by that stage we will have started parsing input, including telnet 
    * control codes.
    */
   telnetWill(TC_ECHO);			// We will echo the incoming characters
   telnetDont(TC_ECHO);			// Client should not echo what we send
   telnetWill(TC_SUPPRESS_GO_AHEAD);	// We will never send a 'GA' character
   telnetDo(TC_SUPPRESS_GO_AHEAD);	// Client should never send us a 'GA'..
   
   
   // Draw the page
   dialogue->drawPage();
}


/* goodbyeSocket - Say goodbye and close the socket down
 * Original 02/01/01, Simon Butcher <simonb@alien.net.au>
 */
void TelnetDescriptor::goodbyeSocket(void)
{
   flags |= ~TELNETFLAG_CONNECTED;
   sock->close();
}


