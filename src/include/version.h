/* version.h
 * Version and Compile information
 */

#ifndef __VERSION_H_
# define __VERSION_H_

# include "person.h"
# include "serverconnection.h"
# include "strings.h"

# define VERNUM_STRING		"0.39"
# define PROGNAME_STRING	"DumbOP"
# define COPYRIGHT_STRING	"(c)1994-2001 alien internet services"

class Version {
 public:
   static String getVersion(void);
   static String getCopyright(void);
   static String getBuild(void);
   static String getCompile(void);
   static void sendInformation(ServerConnection *, Person *);
};

#endif
