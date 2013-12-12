.func bcopy
.synop begin
#include <string.h>
void bcopy( const void *src, void *dst, size_t n );
.ixfunc2 '&String' &func
.ixfunc2 '&Copy' &func
.synop end
.desc begin
The &func function copies the byte string pointed to by
.arg src
(including any null characters) into the array pointed to by
.arg dst
.ct .li .
The number of bytes to copy is specified by
.arg n
.ct .li .
Copying of overlapping objects is guaranteed to work properly.
.np
Note that this function is similar to the ANSI
.kw memmove
function but the order of arguments is different
(new code should use the ANSI function).
.desc end
.return begin
The &func function has no return value.
.return end
.see begin
.seelist &function. bcmp bcopy bzero memmove strcpy
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

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
