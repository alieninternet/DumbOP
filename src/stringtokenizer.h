
#ifndef __STRINGTOKENIZER_H_
#define __STRINGTOKENIZER_H_

#include "string.h"

class StringTokenizer {
  String st;
  int pos;
public:
  StringTokenizer(String);
  StringTokenizer(StringTokenizer &);
  
  bool hasMoreTokens();
  bool hasMoreTokens(char);
  
  int countTokens();
  int countTokens(char);
  
  String nextToken();
  String nextToken(char, bool = false);
  
  String rest();
};

#endif
