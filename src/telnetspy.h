/* telnetspy.h
 * Off-relay spy code for debugging
 */

#ifndef __TELNETSPY_H_
# define __TELNETSPY_H_

# include "str.h"
# include "utils.h" 
# include "bot.h"

class TelnetSpy {
 public:
   static void spyLine(Bot *, String, String, String);

   static String IRCtoANSI(String, String, String, String);

   static String spyHeaderInit();
   static String spyHeaderUpdate();
   
   friend class Utils;
};

#endif
