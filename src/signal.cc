/* signal.cc
 * Signal handling code
 */

#include <signal.h>

#include "signal.h"
#include "bot.h"

/* SigSIGHUP - Handle SIGHUP
 * Original 1/1/01, Pickle <pickle@alien.net.au>
 */
void SigSIGHUP(int)
{
//   logLine("Caught SIGHUP - Cannot do anything :(");
}

/* SetSignals - Set up where signals are handled
 * Original 1/1/01, Pickle <pickle@alien.net.au>
 */
void Signal::SetSignals(void)
{
   // Do a bit of signal cleaning first, just in case
   UnsetSignals();
   
   // Go through the list of signals we "do" or ignore
   signal(SIGHUP, SigSIGHUP);
   signal(SIGPIPE, SIG_IGN);
}
   
/* UnsetSignals - Revert all signals back to their original handlers
 * Original 1/1/01, Pickle <pickle@alien.net.au>
 */
void Signal::UnsetSignals(void)
{
   int i;
   
   for (i = 0; i < NSIG; i++)
     signal(i, SIG_DFL);
}

