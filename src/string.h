
#ifndef __STRING_H_
#define __STRING_H_

#include <iostream.h>

class String {

  struct srep {
    char *s;  // pointer on the data
    int n;    // reference counter
    srep() 
      { n = 1; }
  };
  srep *p;
  int len;
  
public:
  String();
  String(const char *);
  String(const String &);
   String(int);
   String(long);
   String(unsigned long);
  String(char);
  
  String & operator=(const char *);
  String & operator=(const String &);
  
  ~String();

  int length() const;
  int find(char);
  void fill(char);
  String pad(int);
  String prepad(int);
   
  String subString(int);
  String subString(int, int);

  String toLower();
  String toUpper();
  String trim();
  int indexOf(char);

  char & operator[](int);
  const char & operator[](int) const;

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
  
  operator const char *() const;

  friend ostream & operator<<(ostream &, const String &);
  friend istream & operator>>(istream &, String &);
};

#endif
