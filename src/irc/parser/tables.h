/* src/irc/parser/tables.h
 * Parser-Functions tables stuff
 * Copyright (c) 2000-2001 Alien Internet Services
 */

struct parserFunctionStruct {
   char *name;
   void (*function)(Person *, String);
};

extern parserFunctionStruct userParserFunctions[];	// user/tables.cc
