/* ./src/strings/class.cc
 * Primary String class (con/destructors, operators etc)
 * Copyright (c) 2000-2001 Alien Internet Services
 */

#include "strings.h"

#include <string.h>
#include <stdio.h>

/* String - [Various Forms] Setup new "STRING" (convert data to a String)
 * Original 15/12/00, Simon Butcher, <simonb@alien.net.au>
 */
String::String()
{
   p = new srep;
   len = 0;
   p->s = 0;
}

String::String(const char *s)
{
   p = new srep;
   len = strlen(s);
   p->s = new char[len + 1];
   strcpy(p->s, s);
}

String::String(const String & s)
{
   s.p->n++;
   p = s.p;
   len = s.len;
}

String::String(unsigned char i)
{
   char temp[16];
   sprintf(temp, "%d", i);
   
   p = new srep;
   len = strlen(temp);
   p->s = new char[len + 1];
   strcpy(p->s, temp);
}

String::String(int i)
{
   char temp[16];
   sprintf(temp, "%d", i);
   
   p = new srep;
   len = strlen(temp);
   p->s = new char[len + 1];
   strcpy(p->s, temp);
}

String::String(unsigned int i)
{
   char temp[16];
   sprintf(temp, "%d", i);
   
   p = new srep;
   len = strlen(temp);
   p->s = new char[len + 1];
   strcpy(p->s, temp);
}

String::String(long i)
{
   char temp[32];
   sprintf(temp, "%ld", i);
   
   p = new srep;
   len = strlen(temp);
   p->s = new char[len + 1];
   strcpy(p->s, temp);
}

String::String(unsigned long i)
{
   char temp[32];
   sprintf(temp, "%ld", i);
   
   p = new srep;
   len = strlen(temp);
   p->s = new char[len + 1];
   strcpy(p->s, temp);
}

String::String(double i)
{
   char temp[128];
   sprintf(temp, "%f", i);
   
   p = new srep;
   len = strlen(temp);
   p->s = new char[len + 1];
   strcpy(p->s, temp);
}

String::String(char c)
{
   p = new srep;
   p->s = new char[2];
   p->s[0] = c;
   p->s[1] = '\0';
   len = 1;
}

/* ~String - Remove pointers and stuff for string
 * Original 15/12/00, Simon Butcher <simonb@alien.net.au>
 */
String::~String()
{
  if (--p->n == 0) {
    delete[] p->s;
    delete p;
  }
}

/* String Operators - [Various Forms] Control operations on String format
 * Original 15/12/00, Simon Butcher <simonb@alien.net.au>
 */
String &String::operator=(const char *s)
{
   if (p->n > 1) {
      p->n--;
      p = new srep;
   }
   else
     delete[] p->s;
   
   len = strlen(s);
   p->s = new char[len + 1];
   strcpy(p->s, s);
   
   return *this;
}

String &String::operator=(const String &s)
{
   s.p->n++; // protection contre st = st
   if (--p->n == 0) {
      delete[] p->s;
      delete p;
   }
   p = s.p;
   len = s.len;
   return *this;
}

char &String::operator[](long i)
{
   if (i < 0 || len < i)
     return p->s[len + 1]; // String index was out of range

   return p->s[i];
}

const char &String::operator[](long i) const
{
   if (i < 0 || len < i)
     return p->s[len + 1]; // String index was out of range
   
   return p->s[i];
}

bool String::operator==(const char *s) const
{
   return strcmp(p->s, s) == 0;
}

bool String::operator==(const String &s) const
{
   return (p == s.p) || (strcmp(p->s, s.p->s) == 0);
}

bool String::operator!=(const char *s) const
{
   return strcmp(p->s, s) != 0;
}

bool String::operator!=(const String &s) const
{
   return strcmp(p->s, s.p->s) != 0;
}

bool String::operator<(const String &s) const
{
   return strcmp(p->s, s.p->s) < 0;
}

bool String::operator>(const String &s) const
{
   return strcmp(p->s, s.p->s) > 0;
}

bool String::operator<=(const String &s) const
{
   return strcmp(p->s, s.p->s) <= 0;
}

bool String::operator>=(const String &s) const
{
   return strcmp(p->s, s.p->s) >= 0;
}

String String::operator+(const char *s)
{
   char *temp = new char[len + strlen(s) + 1];
   
   strcpy(temp, p->s);
   strcat(temp, s);
   
   String res(temp);
   delete temp;
   
   return res;
}

String String::operator+(const String &s)
{
   char * temp = new char[len + s.len + 1];
   
   strcpy(temp, p->s);
   strcat(temp, s.p->s);
   
   String res(temp);
   delete temp;
   
   return res;
}

String String::operator+=(const char *s)
{
}

String String::operator+=(const String &s)
{
}

String::operator const char *() const
{
   return p->s;
}

String::operator char *() const
{
   return p->s;
}

ostream &operator<<(ostream &s, const String &line)
{
   return s << line.p->s;
}

istream &operator>>(istream &s, String &line)
{
   if (line.p->n > 1) {
      line.p->n--;
      line.p = new String::srep;
   } else
     delete[] line.p->s;
   
   char buf[2048];
   char c;
   
   s.get(buf, 2048, '\n');
   s.get(c);
   
   line.len = strlen(buf);
   line.p->s = new char[line.len + 1];
   strcpy(line.p->s, buf);
   
   return s;
}

