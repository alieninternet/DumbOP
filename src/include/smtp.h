/* src/include/smtp.h
 * SMTP (e-mail) engine routines
 */

#ifndef __SMTP_H_
# define __SMTP_H_

class SMTP;

# include "bot.h"

class SMTP {
   Bot *bot;					// Recursive up
   
   SMTP(Bot *);					// Constructor
   
 public:
   
   
   friend class Bot;
};

#endif
