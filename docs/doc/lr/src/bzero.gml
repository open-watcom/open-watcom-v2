.func bzero
.synop begin
#include <string.h>
void bzero( void *dst, size_t n );
.ixfunc2 '&String' &funcb
.synop end
.desc begin
The
.id &funcb.
function fills the first
.arg n
bytes of the object pointed to by
.arg dst
with zero (null) bytes.
.np
Note that this function is similar to the ANSI
.kw memset
function (new code should use the ANSI function).
.desc end
.return begin
The
.id &funcb.
function has no return value.
.return end
.see begin
.seelist bcmp bcopy bzero memset strset
.see end
.exmp begin
#include <string.h>

void main()
  {
    char buffer[80];
.exmp break
    bzero( buffer, 80 );
  }
.exmp end
.class WATCOM
.system
