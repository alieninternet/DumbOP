/* signals.h
 * Signal handling code
 */

#ifndef __SIGNALS_H_
# define __SIGNALS_H_

class Signals;
class Bot;

# include "bot.h" 

class Signals {
 public:
   Signals(Bot *);
   ~Signals(void);
};

#endif
