/* log.cc
 * Logging routines
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include "log.h"
#include "version.h"

/* Log - Init logfile
 * Original 1/1/01, Pickle <pickle@alien.net.au>
 */
Log::Log(String filename)
  :logFile(open(filename, O_WRONLY | O_CREAT | O_APPEND)),
   flags(LOGFLAG_ALL)
{
   if (!logFile) {
      fprintf(stderr, "Could not open logfile, using Standard-Error instead.");
      logFile = (int)&stderr;
   }

   Log::logRawLine(String("Starting log file (v") + String(VERSION_STRING) + 
		   String(")."));
}

/* ~Log - Deinit logfile
 * Original 1/1/01, Pickle <pickle@alien.net.au>
 */
Log::~Log()
{
   Log::logRawLine("Ending log file.");
   
   if (logFile && (logFile != (int)&stderr))
     close(logFile);
}

/* getLogTime - Convert time into a nice readable string for the log file
 * Original 1/1/01, Pickle <pickle@alien.net.au>
 * Needs: To look like syslog for neatness sake
 */
String getLogTime(void)
{
   time_t current_time = time(NULL);
   struct tm *d;
   
   d = localtime(&current_time);
   
   return String(current_time) + String("|");
}
  
/* logRawLine - Log a raw line of text, without flag checking or anything.
 * Original 1/1/01, Pickle <pickle@alien.net.au>
 */
void Log::logRawLine(String line)
{
//   fprintf(logFile, "%s - %s", getLogTime(), line);
}
