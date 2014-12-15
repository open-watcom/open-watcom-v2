.func jiskigou
.synop begin
#include <jstring.h>
int jiskigou( JMOJI c );
.ixfunc2 '&CharTest' &funcb
.synop end
.desc begin
The
.id &funcb.
function tests if the argument
.arg c
is a double-byte Kigou character (punctuation and other special
graphical symbols).
A double-byte Kigou character is any character for which the following
expression is true:
.millust begin
0x8141 <= c <= 0x81AC  &&  c != 0x817F
.millust end
.desc end
.return begin
The
.id &funcb.
function returns zero if the argument is not a double-byte Kigou
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
    0x817B,
    0x8183,
    0x8190
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
        if( jiskigou( c ) ) {
          printf( " is a kigou symbol" );
        }
        printf( "\n" );
    }
  }
.exmp output
,  is a kigou symbol
+  is a kigou symbol
<  is a kigou symbol
$  is a kigou symbol
.exmp end
.class WATCOM
.system
