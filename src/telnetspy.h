/* telnetspy.h
 * Off-relay spy code for debugging
 */

#ifndef __TELNETSPY_H_
# define __TELNETSPY_H_

#include "string.h"

class TelnetSpy {
 public:
   static void spyLine(String);
};

#endif
