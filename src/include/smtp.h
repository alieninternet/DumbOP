/* src/include/smtp.h
 * SMTP (e-mail) engine routines
 */

#ifndef __SMTP_H_
# define __SMTP_H_

# include <stack.h>	// should be <queue>?
# include <time.h>

class SMTP;

# include "bot.h"

// Address class
class SMTPAddress {
 private:
   String name;					// Name
   String address;				// E-mail address
   
 public:
   SMTPAddress(String, String);			// Class constructor
   
   String getAddress(void) const;		// Get complete address
};

// Message class
class SMTPMessage {
 private:
   SMTPAddress *sender;				// Sender of the message
   SMTPAddress *recipient;			// Recipient of the message

   String subject;				// Subject of the message
   time_t sentTime;				// Time message was 'sent'
   
   queue< String > data;			// Message data FIFO

   unsigned char attempts;			// Number of send attempts
   time_t lastAttempt;				// Time of last send attempt
   
 public:
   SMTPMessage(SMTPAddress *, SMTPAddress *, 
	       String, time_t);			// Set up a new message
   ~SMTPMessage(void);				// Class destructor
   
   void addLine(String);			// Add a line to the data FIFO
};

// Connection class
class SMTPConnection {
   
};

// Main class
class SMTP {
   Bot *bot;					// Recursive up
   
   SMTPAddress *address;			// Our primary address
   
 public:
   SMTP(Bot *);					// Constructor
   ~SMTP(void);					// Class destructor
};

#endif
