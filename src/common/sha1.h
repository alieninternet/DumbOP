/* ./src/sha1.cc
 * SHA1 Message Digest Routines
 * Copyright (c) 2001 Alien Internet Services
 */

#ifndef __SHA1_H_
# define __SHA1_H_

#include "strings.h"

#define USE_LITTLE_ENDIAN /* This should be #define'd if true. */
// #define SHA1HANDSOFF /* Copies data before messing with it. */
// #define SHA1_REVERSE_BYTE_ORDER /* reverse the long byte order */     

// Do not touch unless you know what you are doing
#define SHA1_BASE	84
#define SHA1_BASE_PAD	6			// MUST BE MAX OUTPUT OF BASE
#define PASSWORD_LEN	(SHA1_BASE_PAD * 5)	// Base by 5 longs

typedef struct {
   unsigned long state[5];
   unsigned long count[2];
   char buffer[64];
} SHA1_CTX;

class SHA1 {
 public:
   static void transform(unsigned long state[5], char buffer[64]);
   static void init(SHA1_CTX *context);
   static void update(SHA1_CTX *context, char *data, unsigned int len);
   static void final(char digest[20], SHA1_CTX *context);
};
   
#endif
