.func jisspace
#include <jstring.h>
int jisspace( JMOJI c );
.ixfunc2 '&CharTest' &func
.funcend
.desc begin
The &func function tests if the argument
.arg c
is a double-byte space character (0x8140).
.desc end
.return begin
The &func function returns zero if the argument is not a double-byte space
character;
otherwise, a non-zero value is returned.
.return end
.see begin
.im seejto &function.
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>

JMOJI chars[] = {
    0x8140,
    0x8183,
    0x8254,
    0x8277
};
.exmp break
#define SIZE sizeof( chars ) / sizeof( JMOJI )
.exmp break
void main()
  {
    int   i;
    JMOJI c;
.exmp break
    for( i = 0; i < SIZE; i++ ) {
        c = chars[ i ];
        printf( "%c%c", c>>8, c );
        if( jisspace( c ) ) {
          printf( " is a space character" );
        }
        printf( "\n" );
    }
  }
.exmp output
   is a space character
<
5
X
.exmp end
.class WATCOM
.system
