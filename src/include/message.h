#ifndef __MESSAGE_H_
#define __MESSAGE_H_

#include "strings.h"

struct Message {
  int code;
  String description;

  Message(int c, String d) : code(c), description(d) { }
  int getCode() const { return code; }
  String getMessage() const { return description; }
};

#endif
