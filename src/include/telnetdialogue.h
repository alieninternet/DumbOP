
#ifndef __TELNETDIALOGUE_H_
# define __TELNETDIALOGUE_H_

class TelnetDialogue;
class TelnetDescriptor;

# include "telnet.h"

// Base class
class TelnetDialogue {
 private:
   TelnetDescriptor *desc;			// Recursive back
   enum {
      LOGIN,
      MONITOR
   };
   char type;					// Type of dialogue
   
   unsigned char boxX, boxY;			// Box starting vector
   
 public:
   TelnetDialogue(TelnetDescriptor *, char);	// Default constructor
   virtual ~TelnetDialogue(void);		// Fake destructor
   
   virtual void drawPage(void);			// Fake routines
   virtual void parseInput(char);

   void calcBoxTLHC(unsigned char, unsigned char);
   void clearWindow(void);
   
   friend class Telnet;
   friend class TelnetDescriptor;
   friend class TelnetDialogueLogin;
   friend class TelnetDialogueMonitor;
};

// Login page - the first page
class TelnetDialogueLogin : public TelnetDialogue {
 private:
   enum {
      NETWORK,
      NICKNAME,
      PASSWORD
   } field;					// Which field are we at?

   String network;				// Network
   String nickname;				// Nickname
   String buff;					// He's buff, 'n' he's cool!

   unsigned char attempts;			// Number of attempts to log in
   
 public:
   TelnetDialogueLogin(TelnetDescriptor *);	// Constructor
   
   void drawPage(void);				// Draw the login box
   void parseInput(char);			// Read an incoming char
};

// Monitor page - Used for monitoring IRC activity/logging etc
class TelnetDialogueMonitor : public TelnetDialogue {
 private:

 public:
   TelnetDialogueMonitor(TelnetDescriptor *);	// Constructor
   
   void drawPage(void);				// Draw the login box
   void parseInput(char);			// Read an incoming char
};

#endif
