
#include "socket.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

Socket::Socket()
  : remotePort(0), remoteAddress(0),
    localPort(0), localAddress(0)
{
  fd = new s_fd;
  fd->fd = socket(AF_INET, SOCK_STREAM, 0);
}

Socket::Socket(int newfd, unsigned long newAddress, int newPort)
  : remotePort(newPort), remoteAddress(newAddress),
    localPort(0), localAddress(0)
{
  fd = new s_fd;
  fd->fd = newfd;
}

Socket::Socket(const Socket &s)
  : remotePort(s.remotePort), remoteAddress(s.remoteAddress),
    localPort(s.localPort), localAddress(s.localAddress)
{
  s.fd->n++;
  fd = s.fd;
}

Socket::~Socket()
{
  if (--fd->n == 0) {
    close();
    delete fd;
  }
}

int
Socket::getRemotePort() const
{
  return remotePort;
}

int
Socket::getLocalPort() const
{
  return localPort;
}

int
Socket::getFileDescriptor() const
{
  return fd->fd;
}

bool
Socket::isConnected()
{
  return fd->fd != -1;
}

bool
Socket::setRemoteHostname(String hostname)
{
  struct hostent *host;

  if ((host = gethostbyname((const char *)hostname)) == 0)
    return false;

  memcpy(&remoteAddress, host->h_addr, host->h_length);
  remoteAddress = ntohl(remoteAddress);

  return true;
}

bool 
Socket::setRemoteIP(unsigned long address)
{
  remoteAddress = address;
  return true;
}

bool
Socket::setRemotePort(int p)
{
  remotePort = p;

  return true;
}

bool
Socket::setLocalHostname(String hostname)
{
  struct hostent *host;

  if ((host = gethostbyname((const char *)hostname)) == 0)
    return false;

  memcpy(&localAddress, host->h_addr, host->h_length);

  struct sockaddr_in local_addr;
  local_addr.sin_family = AF_INET;
  local_addr.sin_port = htons(localPort);
  local_addr.sin_addr.s_addr = htonl(localAddress);
  if (bind(fd->fd, (struct sockaddr *)(&local_addr), sizeof(local_addr)) < 0)
    return false;

  return true;
}

bool 
Socket::setLocalIP(unsigned long address)
{
  localAddress = address;

  struct sockaddr_in local_addr;
  local_addr.sin_family = AF_INET;
  local_addr.sin_port = htons(localPort);
  local_addr.sin_addr.s_addr = htonl(localAddress);
  if (bind(fd->fd, (struct sockaddr *)(&local_addr), sizeof(local_addr)) < 0)
    return false;

  return true;
}

bool
Socket::setLocalPort(int p)
{
  localPort = p;

  struct sockaddr_in local_addr;
  local_addr.sin_family = AF_INET;
  local_addr.sin_port = htons(localPort);
  local_addr.sin_addr.s_addr = htonl(localAddress);
  if (bind(fd->fd, (struct sockaddr *)(&local_addr), sizeof(local_addr)) < 0)
    return false;

  return true;
}

bool
Socket::setNonBlocking(bool nonblock)
{
  long flags;

  if (fd->fd == -1)
    return false;

  // We first get the file descriptor's flags
  if (!fcntl(fd->fd, F_GETFL, &flags))
    return false;

  if (nonblock)
    return fcntl(fd->fd, F_SETFL, flags | O_NONBLOCK);
  else
    return fcntl(fd->fd, F_SETFL, flags & ~O_NONBLOCK);
}

bool
Socket::connect()
{
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(remoteAddress);
  addr.sin_port = htons(remotePort);
  if (::connect(fd->fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    return false;
  
  return true;
}

bool
Socket::listen(int backlog)
{
  return ::listen(fd->fd, backlog) == 0;
}

Socket
Socket::accept()
{
  struct sockaddr_in addr;
  socklen_t addrlen=sizeof(addr);
  int newfd = ::accept(fd->fd, (sockaddr *)&addr, &addrlen);
  if (newfd == -1)
    return Socket(-1, 0, 0);
  unsigned long newRemoteAddress = ntohl(addr.sin_addr.s_addr);
  int newRemotePort = ntohs(addr.sin_port);
  return Socket(newfd, newRemoteAddress, newRemotePort);
}

void
Socket::close()
{
  ::close(fd->fd);
  fd->fd = -1;
}

bool
Socket::write(String s, bool m)
{
  if (fd->fd == -1)
    return false;
  
  if (m) {
    if (::write(fd->fd, (const char *)s, s.length()) +
        ::write(fd->fd, "\n", 1) != s.length() + 1)
      return false;
  }
  else
    if (::write(fd->fd, (const char *)s, s.length()) +
        ::write(fd->fd, "\r\n", 2) != s.length() + 2)
      return false;

  return true;
}

String
Socket::readLine()
{
  static char buf[512];
  int pos = 0, nb;
  char r;
  
  do {
    nb = ::read(fd->fd, &r, 1);
    switch (nb) {
    case 0:
      return String("");
    case -1:
      if (errno != EINTR && errno != EAGAIN)
        return String("");
      sleep(1);
    }

    if (nb != -1)
      buf[pos++] = r;
  } while (r != '\n');
  
  if (pos > 1 && buf[pos-2] == '\r')
    buf[pos-2] = '\0';
  else
    buf[pos-1] = '\0';

  return String(buf);
}

String
Socket::readChar()
{
  char r[2];
  int nb;
  nb = ::read(fd->fd, &r, 1);
  switch (nb) {
  case 0:
    return String("");
  case -1:
    if(errno != EINTR && errno != EAGAIN)
      return String("");
    sleep(1);
  }
  r[1] = '\0';
  return String(r);
}


// // Here we use a circular buffer to read from the socket
// String
// Socket::readLine()
// {
//   // The result string
//   char result[512];
//   // Position in the result string
//   int pos = 0;
//   // Number of chars read from the socket
//   int nb;

//   // First, we read from the socket
//  beginning:
//   if (end <= begin) {
//     nb = ::read(fd->fd, buf+end, begin - end);
//   } else {
//     nb = ::read(fd->fd, buf+end, 1024 - end);
//   }

//   if (nb == -1 && errno != EINTR && errno != EAGAIN)
//     return String("");

//   end = (end + nb) % 1024;

//   // Fine, now we read our string from the buffer
//   while (buf[begin] != '\r' && buf[begin] != '\n') {
//     result[pos++] = buf[begin++];
//     begin %= 1024;
//     if (begin == end)
//       goto beginning;
//   }

//   result[pos] = '\0';

//   // Now we skip the final '\r' and '\n'
//   if (buf[begin] == '\r')
//     begin = (begin + 1) % 1024;
//   if (buf[begin] == '\n')
//     begin = (begin + 1) % 1024;

//   // And we return the result
//   return String(result);
// }

// bool
// Socket::hasData()
// {
//   cout << "DEBUG hasData = " << (begin != end) << endl;
//   return begin != end;
// }
