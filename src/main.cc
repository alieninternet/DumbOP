
#include <unistd.h>
#include <iostream.h>
#include <stdio.h>
#include <signal.h>

#ifdef USESCRIPTS
extern "C" {
#include <guile/gh.h>
}
#include "Interp.H"
#endif

#include "bot.h"

Bot *b;

void sig_hup(int) {
  if (b) {
    b->userList->read();
//    b->userList->addUserFirst("bidon", "bidon", 0, 0, false, 0, -1, "");
    b->reconnect();
  }
}

void printUsage(char *name)
{
  cout << "Usage: " << name << " [-h] [-b] [-f file] [-d dir] [-D]\n"
    " -h        Shows this help.\n"
    " -b        Do not run in background.\n"
    " -f file   Use file as config file.\n"
    " -d dir    Use dir as current dir.\n"
    " -D        Debug mode (input/output printing and no background mode.\n";
}

int main(int argc, char **argv)
{
  int opt;
  extern char *optarg;
  bool background = true;
  String configFile = "bot.conf";
  String directory = "";
  bool debug = false;

  signal(SIGPIPE, SIG_IGN);
  signal(SIGALRM, SIG_IGN);
  signal(SIGHUP,  sig_hup);

  // We parse the command line options
  while ((opt = getopt(argc,argv,"hbf:d:D")) != EOF)
    switch (opt) {
    case 'h':
      printUsage(argv[0]); exit(0);
    case 'b':
      background = false;
      break;
    case 'f':
      configFile = optarg;
      break;
    case 'd':
      directory = optarg;
      break;
    case 'D':
      debug = true;
      break;
    default:
      printUsage(argv[0]); exit(1);
    }

  if ((directory != "") && (chdir((const char *)directory)<0))
    perror("Warning: ");

  if (!debug) {
    if (background)
      switch (fork()) {
      case -1:
        cout << "Could not run in the background. Exiting...\n";
        perror("fork");
        exit(1);
      case 0:
        break;
      default:
        cout << "Running in the background...\n";
        exit(0);
      }
  }

  b = new Bot(configFile, debug);
  b->run();
  delete b;

   return 0;
}

