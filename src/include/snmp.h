/* src/include/snmp.h
 * SNMP (network management) node routines
 */

#ifndef __SNMP_H_
# define __SNMP_H_

class SNMP;

# include "bot.h"

class SNMP {
   Bot *bot;					// Recursive up
   
   SNMP(Bot *);					// Constructor
   
 public:
   
   
   friend class Bot;
};

#endif
