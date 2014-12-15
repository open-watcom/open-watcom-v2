.func jisupper
.synop begin
#include <jstring.h>
int jisupper( JMOJI c );
.ixfunc2 '&CharTest' &funcb
.synop end
.desc begin
The
.id &funcb.
function tests if the argument
.arg c
is an uppercase double-byte character.
These are any characters for which the following expression is true:
.millust begin
0x8260 <= c <= 0x8279
.millust end
.np
These are double-byte codes for the characters 'A' to 'Z'.
.desc end
.return begin
The
.id &funcb.
function returns zero if the argument is not an uppercase double-byte
character;
otherwise, a non-zero value is returned.
.return end
.see begin
.im seejto
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
        if( jisupper( c ) ) {
          printf( " is an uppercase character" );
        }
        printf( "\n" );
    }
  }
.exmp output
,
<
5
X  is an uppercase character
.exmp end
.class WATCOM
.system
