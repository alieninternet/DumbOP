
#ifndef __MASK_H_
#define __MASK_H_

#include "string.h"

class Mask {
  String mask;

public:
  Mask(String);
  Mask(Mask &);

  String getMask();
  
  bool matches(String);
  bool matches(Mask &);

protected:
  bool match(const char *, const char *);
};

#endif
