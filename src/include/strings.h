
#ifndef __STRINGS_H_
# define __STRINGS_H_

#include <iostream.h>

class String {
   struct srep {
      char *s;  // pointer on the data
      long n;    // reference counter
      srep() 
	{ n = 1; }
   };
   srep *p;
   long len;
   
 public:
   String();
   String(const char *);
   String(const String &);
   String(unsigned char);
   String(int);
   String(unsigned int);
   String(long);
   String(unsigned long);
   String(double);
   String(char);
   
   String & operator=(const char *);
   String & operator=(const String &);
   
   ~String();
   
   int length() const;
   int find(char);
   void fill(char);
   String pad(long);
   String prepad(long, char = ' ');
   
   String subString(long);
   String subString(long, long);

   String getFirst(void);
   
   String toLower();
   String toUpper();

   int toInt(void);
   long toLong(void);
   double toDouble(void);
   
   String trim();
   int indexOf(char);

   String safetyEscape();
   String safetyUnescape();
   
   char & operator[](long);
   const char & operator[](long) const;
   
   bool operator==(const char *) const;
   bool operator==(const String &) const;
   
   bool operator!=(const char *) const;
   bool operator!=(const String &) const;
   
   bool operator<(const String &) const;
   bool operator>(const String &) const;
   
   bool operator<=(const String &) const;
   bool operator>=(const String &) const;
   
   String operator+(const char *);
   String operator+(const String &);

   String operator+=(const char *);
   String operator+=(const String &);

   operator const char *() const;
   operator char *() const;

   friend ostream & operator<<(ostream &, const String &);
   friend istream & operator>>(istream &, String &);
};

// StringTokens for strings
class StringTokens {
   String st;
   long pos;
 public:
   StringTokens(String);
   StringTokens(StringTokens &);
   
   bool hasMoreTokens();
   bool hasMoreTokens(char);
   
   long countTokens();
   long countTokens(char);
   
   String nextToken();
   String nextToken(char, bool=false);
   
   String rest();
};

#endif
