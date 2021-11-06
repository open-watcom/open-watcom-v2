.func bzero
.synop begin
.deprec
#include <strings.h>
void bzero( void *dst, size_t n );
.ixfunc2 '&String' bzero
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
Note that this function is similar to the ISO C
.reffunc memset
function (new code should use the ISO C function).
.desc end
.return begin
The
.id &funcb.
function has no return value.
.return end
.see begin
.seelist memcmp memcpy memset _strset
.see end
.exmp begin
#include <strings.h>

void main()
  {
    char buffer[80];
.exmp break
    bzero( buffer, 80 );
  }
.exmp end
.class WATCOM
.system
