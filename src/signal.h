/* signal.h
 * Signal handling code
 */

#ifndef __SIGNAL_H_
# define __SIGNAL_H_

class Signal {
 public:
   static void SetSignals(void);
   static void UnsetSignals(void);
};

#endif
