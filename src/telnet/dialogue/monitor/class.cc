/* class.cc
 * Monitor dialogue box, main class file
 */

#include "config.h"

#include "telnetdialogue.h"
#include "ansi.h"
#include "utils.h"
#include "version.h"


/* TelnetDialogueMonitor - Monitor sub-class constructor
 * Original 01/08/01, Simon Butcher <simonb@alien.net.au>
 */
TelnetDialogueMonitor::TelnetDialogueMonitor(TelnetDescriptor *d)
: TelnetDialogue(d, MONITOR)
{
   // Turn off teletype mode
   desc->flags &= ~TELNETFLAG_TELETYPE;
}
