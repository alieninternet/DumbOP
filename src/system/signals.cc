/* signals.cc
 * Signal handling code
 */

#include <signal.h>

#include "signals.h"
#include "commands.h"

static const char *sigNames[] = {
   "SIGZERO",   //  0 - Apparently never used
   "SIGHUP",    //  1 - Hang up detected on controlling terminal
   "SIGINT",    //  2 - Interrupt from keyboard (^C)
   "SIGQUIT",   //  3 - Quit from keyboard (^D)
   "SIGILL",    //  4 - Illegal intruction called
   "SIGTRAP",   //  5 - Trace/Breakpoint trap
   "SIGABRT",   //  6 - Abort signal from abort()
   "SIGEMT",    //  7 -
   "SIGFPE",    //  8 - Floating point exception
   "SIGKILL",   //  9 - Kill signal (cannot be caught)
   "SIGBUS",    // 10 - BUS error (bad memory access, usually causes core dump)
   "SIGSEGV",   // 11 - Invalid memory reference (segment violation)
   "SIGSYS",    // 12 - Bad argument to routine
   "SIGPIPE",   // 13 - Broken pipe (written to pipe with nobody listening)
   "SIGALRM",   // 14 - Time signal from alarm()
   "SIGTERM",   // 15 - Termination signal
   "SIGURG",    // 16 - Urgent condition on socket (should induce race condition)
   "SIGSTOP",   // 17 - Stop process (pauses process, cannot be caught)
   "SIGTSTP",   // 18 - Stop typed at TTY
   "SIGCONT",   // 19 - Continue if stopped
   "SIGCHLD",   // 20 - Child process stopped or terminated
   "SIGTTIN",   // 21 - TTY input for background process
   "SIGTTOU",   // 22 - TTY output for background process
   "SIGIO",     // 23 - I/O now possible
   "SIGXCPU",   // 24 - CPU time limit exceeded (usually terminates with core dump)
   "SIGXFSZ",   // 25 - File size limit exceeded (usually terminates with core dump)
   "SIGVTALRM", // 26 - Virtual alarm clock
   "SIGPROF",   // 27 - Profiling timer expired
   "SIGWINCH",  // 28 - Window resize signal
#ifdef SIGLOST
   "SIGLOST",   // 29 - Signal Lost (apparently obsolete)
#else
   "SIGPWR",    // 29 - Power failure
#endif
   "SIGUSR1",   // 30 - User-defined signal 1
   "SIGUSR2"    // 31 - User-defined signal 2
};
   
Bot *bot; // We need this so we can do stuff during a signal event

/* logSignal - Log the catchment of a signal
 * Original 11/1/01, Simon Butcher <simonb@alien.net.au>
 */
void logSignal(unsigned char sig, String action = "")
{
   bot->logLine(String("Caught Signal ") + String(sigNames[sig]) +
		((action.length() == 0) ? String("") : 
		 (String(": ") + action)));
}


/* sigHandler - Handle signals
 * Original 11/1/01, Simon Butcher <simonb@alien.net.au>
 */
//RETSIGTYPE sigHandler(int sig)
void sigHandler(int sig)
{
   switch (sig) {
      // Reload files
    case SIGHUP:
      logSignal(sig, "Should be reloading files.");
      break;
      
      // Panic state - dump, restart
    case SIGILL:
    case SIGTRAP:
#ifdef SIGEMT
    case SIGEMT:
#endif
#ifdef SIGBUS
    case SIGBUS:
#endif
#ifdef SIGSEGV
    case SIGSEGV:
#endif
#ifdef SIGSYS
    case SIGSYS:
#endif
#ifdef SIGURG
    case SIGURG:
#endif
    case SIGFPE:
      exit(250);
      break;

      // Normal shutdown
    case SIGINT:
    case SIGQUIT:
    case SIGPIPE:
    case SIGTERM:
    case SIGABRT:
#ifdef SIGXCPU
    case SIGXCPU:
#endif
#ifdef SIGXFSZ
    case SIGXFSZ:
#endif
      logSignal(sig, "Shutting down.");
      Commands::Die(bot, String("Caught ") + String(sigNames[sig]) +
		    String(": Shutting down. [") + 
		    VERSION_STRING + String("]"));
      break;
      
      // Everything else we ignore.
    default:
      logSignal(sig, "Ignoring (Perplexed)");
   }
   
   // Reset the signal for future handling
   signal(sig, sigHandler);
}


/* Signals - Set up where signals are handled
 * Original 1/1/01, Pickle <pickle@alien.net.au>
 */
Signals::Signals(Bot *b)
{
   // Make a pointer to the bot so we know where to get it later
   bot = (Bot *)b;
   
   // We are very multi-talented, we handle it all!
   for (unsigned char i = 0; i < NSIG; i++)
     signal(i, sigHandler);
}
   

/* ~Signals - Revert all signals back to their original handlers
 * Original 1/1/01, Pickle <pickle@alien.net.au>
 */
Signals::~Signals(void)
{
   for (unsigned char i = 0; i < NSIG; i++)
     signal(i, SIG_DFL);
}

