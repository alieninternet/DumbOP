/* version.h
 * Version and Compile information
 */

#ifndef __VERSION_H_
# define __VERSION_H_

# include "person.h"
# include "serverconnection.h"

# define VERNUM_STRING		"0.37"
# define PROGNAME_STRING	"DumbOP"
# define VERSION_STRING		String(PROGNAME_STRING) + String(" v") + String(VERNUM_STRING)
# define COPYRIGHT_STRING	"(c)1994-2001 alien internet services"

# ifndef COMPILE_STRING
#  define COMPILE_STRING	"(void)"
# endif
# ifndef BUILD_STRING
#  define BUILD_STRING		"(void)"
# endif

class Version {
 public:
   static void sendInformation(ServerConnection *, Person *);
};

#endif
