/* main.c
 * The beginning, and the end, but not the middle
 */

#include <unistd.h>
#include <iostream.h>
#include <stdio.h>

#include "bot.h"

Bot *b;

void printUsage(char *name)
{
   cout << "Usage: " << name << " [-h] [-b] [-f file] [-d dir] [-D]\n"
     " -h        Shows this help.\n"
     " -b        Do not run in background.\n"
     " -f file   Use file as config file.\n"
     " -d dir    Use dir as current dir.\n";
#ifdef DEBUG
   cout << " -D        Debug mode (input/output printing and no background mode.\n";
#endif
}

int main(int argc, char **argv)
{
   int opt;
   extern char *optarg;
   bool background = true;
   String configFile = "bot.conf";
   String directory = "";
#ifdef DEBUG
   bool debug = false;
#endif
   
   // We parse the command line options
   while ((opt = getopt(argc,argv,"hbf:d:D")) != EOF)
     switch (opt) {
      case 'h':
	printUsage(argv[0]); 
	exit(0);
	break;
      case 'b':
	background = false;
	break;
      case 'f':
	configFile = optarg;
	break;
      case 'd':
	directory = optarg;
	break;
#ifdef DEBUG
      case 'D':
	debug = true;
	break;
#endif
      default:
	printUsage(argv[0]); 
	exit(1);
	break;
     }
   
   if ((directory != "") && (chdir((const char *)directory)<0))
     perror("Warning: ");
   
#ifdef DEBUG
   if (!debug) {
#endif
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
#ifdef DEBUG
   }
#endif
   
   // Initialise the bot
#ifdef DEBUG
   b = new Bot(configFile, debug);
#else
   b = new Bot(configFile);
#endif

   // Shoot
   b->run();
   
   // The bot returned, kill it off
   delete b;
   
   return 0;
}

