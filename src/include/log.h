/* log.h
 * Logging routines
 */

#ifndef __LOG_H_
# define __LOG_H_

# include <stdio.h>

# include "strings.h"

# define LOGFLAG_IRC		0x0001	// Log IRC server stuff
# define LOGFLAG_ALL		0xFFFF	// Log everything and anything

class Log {
   int logFile;
   int flags;
   
 public:
   Log(String);
   ~Log();

   void logRawLine(String);
};

#endif
