/* src/telnet/dialogue/class.cc
 * Telnet Dialogue handler class
 */

#include "config.h"
#include "telnetdialogue.h"


/* TelnetDialogue - Dialogue base class constructor
 * Original 28/07/01, Simon Butcher <simonb@alien.net.au>
 */
TelnetDialogue::TelnetDialogue(TelnetDescriptor *d, char t)
: desc(d),
  type(t)
{
}


/* ~TelnetDialogue - Virtual class destructor
 * Original 29/07/01, Simon Butcher <simonb@alien.net.au>
 */
TelnetDialogue::~TelnetDialogue(void)
{
   // This is virtual. Nothing should go here.
}


