
#include "config.h"

#include <iostream.h>

#include "queue.h"

/* Queue - Initialise Queuing stuff
 * Original 10/12/00, Pickle <pickle@alien.net.au>
 */
#ifdef DEBUG
Queue::Queue(Socket *s, bool d)
  : sock(s), debug(d)
#else
Queue::Queue(Socket *s)
  : sock(s)
#endif
{
}

/* ~Queue - Deinit Queuing stuff
 * Original 10/12/00, Pickle <pickle@alien.net.au>
 */
Queue::~Queue()
{
}

/* sendLine - Send a line to the server
 * Original 10/12/00, Pickle <pickle@alien.net.au>
 */
bool Queue::sendLine(String s)
{
#ifdef DEBUG
   if (debug)
     cout << "> " << s << endl;
#endif
   
   return sock->writeln(s);
}
