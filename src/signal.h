/* signal.h
 * Signal handling code
 */

#ifndef __SIGNAL_H_
# define __SIGNAL_H_

class Signal;

# include "bot.h" 

class Signal {
 public:
   Signal(Bot *);
   ~Signal(void);
};

#endif
