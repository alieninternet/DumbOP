
#ifndef __SOCKET_H_
# define __SOCKET_H_

# include "string.h"

class Telnet;

class Socket {
  struct s_fd {
    int fd; // the data
    int n; // reference counter
    s_fd() { n = 1; }
  };
  s_fd *fd;

  int remotePort;
  unsigned long remoteAddress;

  int localPort;
  unsigned long localAddress;

  int begin, end;

  char buf[1024];

public:
  Socket();
  Socket(int, unsigned long, int);
  Socket(const Socket &);
  Socket(Socket &);
   
  ~Socket();

  int getRemotePort() const;
  int getLocalPort() const;
  int getFileDescriptor() const;

  bool isConnected();

  bool setRemoteHostname(String);
  bool setRemoteIP(unsigned long);
  bool setRemotePort(int);

  bool setLocalHostname(String);
  bool setLocalIP(unsigned long);
  bool setLocalPort(int);

  bool setNonBlocking(bool = true);

  bool bindPort(int);
  bool connect();
  bool listen(int = 5);
  Socket accept();
  void close();
  
  bool writeln(String, bool = false);
  bool write(String);

  String readLine();
  String readChar();
  bool hasData();
};

#endif
