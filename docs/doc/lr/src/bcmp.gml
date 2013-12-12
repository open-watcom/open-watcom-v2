.func bcmp
.synop begin
#include <string.h>
int bcmp(const void *s1, const void *s2, size_t n);
.ixfunc2 '&String' &func
.ixfunc2 '&Compare' &func
.synop end
.desc begin
The
.id &func.
function compares the byte string pointed to by
.arg s1
to the string pointed to by
.arg s2
.ct .li .
The number of bytes to compare is specified by
.arg n
.ct .li .
Null characters may be included in the comparision.
.np
Note that this function is similar to the ANSI
.kw memcmp
function but just tests for equality
(new code should use the ANSI function).
.desc end
.return begin
The
.id &func.
function returns zero if the byte strings are identical;
otherwise it returns 1.
.return end
.see begin
.seelist &function. bcmp bcopy bzero memcmp strcmp
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

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
