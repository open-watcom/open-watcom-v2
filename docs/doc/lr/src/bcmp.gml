.func bcmp
.synop begin
.deprec
#include <strings.h>
int bcmp(const void *s1, const void *s2, size_t n);
.ixfunc2 '&String' bcmp
.ixfunc2 '&Compare' bcmp
.synop end
.desc begin
The
.id &funcb.
function compares the byte string pointed to by
.arg s1
to the string pointed to by
.arg s2
.period
The number of bytes to compare is specified by
.arg n
.period
Null characters may be included in the comparision.
.np
Note that this function is similar to the ISO C
.reffunc memcmp
function but just tests for equality
(new code should use the ISO C function).
.desc end
.return begin
The
.id &funcb.
function returns zero if the byte strings are identical;
otherwise it returns 1.
.return end
.see begin
.seelist bcmp bcopy bzero memcmp strcmp
.see end
.exmp begin
#include <stdio.h>
#include <strings.h>

void main()
  {
    if( bcmp( "Hello there", "Hello world", 6 ) ) {
      printf( "Not equal\n" );
    } else {
      printf( "Equal\n" );
    }
  }
.exmp output
Equal
.exmp end
.class WATCOM
.system
