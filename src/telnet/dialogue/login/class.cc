/* src/telnet/dialogue/login/class.cc
 * Login dialogue box, main class file
 */

#include "config.h"
#include "login.h"

#include "telnetdialogue.h"
#include "ansi.h"
#include "utils.h"
#include "version.h"

// This is a table of the three fields we have. Cheap way of saving memory.
const char *fields[] = {
   "Network:", // Note the +1 char offset.
   "Nickname:",
   "Password:"
};

/* TelnetDialogueLogin - Login sub-class constructor
 * Original 28/07/01, Simon Butcher <simonb@alien.net.au>
 */
TelnetDialogueLogin::TelnetDialogueLogin(TelnetDescriptor *d)
: TelnetDialogue(d, LOGIN)
{
   // Reset some variables
   field = NETWORK;
   network = nickname = buff = "";
   attempts = 0;

   // We should be in non-masking teletype mode.
   desc->flags |= TELNETFLAG_TELETYPE | TELNETFLAG_TELETYPE_NO_BS;
   desc->flags &= ~TELNETFLAG_TELETYPE_MASK;
}

