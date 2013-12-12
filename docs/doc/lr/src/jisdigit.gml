.func jisdigit
.synop begin
#include <jstring.h>
int jisdigit( JMOJI c );
.ixfunc2 '&CharTest' &func
.synop end
.desc begin
The &func function tests if the argument
.arg c
is a double-byte character equivalent to the single-byte decimal digit
characters "0" to "9".
A double-byte decimal digit character is any character for which the
following expression is true:
.millust begin
0x824F <= c <= 0x8258
.millust end
.desc end
.return begin
The &func function returns zero if the argument is not a double-byte decimal
digit character;
otherwise, a non-zero value is returned.
.return end
.see begin
.im seejto &function.
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>

JMOJI chars[] = {
    0x8143,
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
        if( jisdigit( c ) ) {
          printf( " is a digit" );
        }
        printf( "\n" );
    }
  }
.exmp output
,
<
5  is a digit
X
.exmp end
.class WATCOM
.system
