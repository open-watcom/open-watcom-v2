/*
  a large struct in any 8086 model
*/
#include <limits.h>
#if INT_MAX == LONG_MAX
typedef char big[65536];
#else
typedef char big;
#endif
struct C {
    big f1[32768];
    big f2[32768];
    big f3[32768];
};
unsigned x = sizeof( struct C );
