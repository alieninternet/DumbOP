/* src/smtp/message.cc
 * SMTP (email) message class
 */

#include "config.h"
#include "smtp.h"


/* SMTPMessage - Create a new message
 * Original 05/08/01, Simon Butcher <simonb@alien.net.au>
 */
SMTPMessage::SMTPMessage(SMTPAddress *s, SMTPAddress *r, String sub,
			 time_t st)
: sender(s),
  recipient(r),
  subject(sub),
  sentTime(st),
  attempts(0),
  lastAttempt(0)
{
   // Reset the data FIFO, just in case
   while (!data.empty()) {
      data.pop();
   }
}


/* ~SMTPMessage - Destroy this message
 * Original 05/08/01, Simon Butcher <simonb@alien.net.au>
 */
SMTPMessage::~SMTPMessage(void)
{
   // Kill off the addresses
   delete sender;
   delete recipient;
}


/* addLine - Add a line to the bottom of the message data FIFO
 * Original 05/08/01, Simon Butcher <simonb@alien.net.au>
 */
void SMTPMessage::addLine(String line)
{
   data.push(line);
}

