/* version.h
 * Version and Compile information
 */

#ifndef __VERSION_H_
# define __VERSION_H_

# include "person.h"
# include "serverconnection.h"

# define VERNUM_STRING		"0.32"
# define PROGNAME_STRING	"DumbOP"
# define VERSION_STRING		String(PROGNAME_STRING) + String(" version ") + String(VERNUM_STRING)
# define COPYRIGHT_STRING	"Copyright (c) 2000 Alien Internet Services <dumbop@alien.net.au>"

# ifndef COMPILE_STRING
#  define COMPILE_STRING	"Void"
# endif

class Version {
 public:
   static void sendInformation(ServerConnection *, Person *);
};

#endif
