/* src/include/garbage.h
 * 'Garbage Man' routines
 */

#ifndef __GARBAGE_H_
# define __GARBAGE_H_

class Garbage;

# include "bot.h"

class Garbage {
   Bot *bot;					// Recursive up
   
   Garbage(Bot *);				// Constructor
   
 public:
   
   
   friend class Bot;
};

#endif
