
#ifndef __PERSON_H_
# define __PERSON_H_

# include "strings.h"
# include "userlistitem.h"
# include "serverconnection.h"

class Person {
 protected:
   String address;				// User mask (x!y@z)
   
 public:
   ServerConnection *cnx;			// Recursive back
   UserListItem *uli;				// Pointer to the user entry
   
   Person(String, ServerConnection *, UserListItem *);
   Person(Person &);
   
   virtual ~Person() { }
   
   virtual Person *copy();
   
   String getNick() const;
   String getAddress() const;
   
   virtual void keepAlive() { }
   
   virtual void sendNotice(String);
   virtual void sendPrivmsg(String);
   virtual void sendCTCP(String, String);
   virtual void sendCTCPReply(String, String);
   
   void sendLine(String);			// Send user a line of text
   
   Person & operator=(const String &);
};

#endif
