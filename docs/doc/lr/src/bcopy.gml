.func bcopy
.synop begin
.deprec
#include <strings.h>
void bcopy( const void *src, void *dst, size_t n );
.ixfunc2 '&String' bcopy
.ixfunc2 '&Copy' bcopy
.synop end
.desc begin
The
.id &funcb.
function copies the byte string pointed to by
.arg src
(including any null characters) into the array pointed to by
.arg dst
.period
The number of bytes to copy is specified by
.arg n
.period
Copying of overlapping objects is guaranteed to work properly.
.np
Note that this function is similar to the ISO C
.reffunc memmove
function but the order of arguments is different
(new code should use the ISO C function).
.desc end
.return begin
The
.id &funcb.
function has no return value.
.return end
.see begin
.seelist bcmp bcopy bzero memmove strcpy
.see end
.exmp begin
#include <stdio.h>
#include <strings.h>

void main()
  {
    auto char buffer[80];
.exmp break
    bcopy( "Hello ", buffer,     6 );
    bcopy( "world",  &buffer[6], 6 );
    printf( "%s\n", buffer );
  }
.exmp output
Hello world
.exmp end
.class WATCOM
.system
