
#ifndef __DCCPARSER_H_
#define __DCCPARSER_H_

#include "string.h"
#include "dccconnection.h"

class DCCParser {
public:
  static void parseLine(DCCConnection *, String);
};

#endif
