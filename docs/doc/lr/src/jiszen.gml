.func jiszen
#include <jstring.h>
int jiszen( JMOJI c );
.ixfunc2 '&CharTest' &func
.funcend
.desc begin
The &func function tests if the argument
.arg c
is a valid double-byte character.
Valid double-byte characters are those in which the first byte falls
in the range 0x81 - 0x9F or 0xE0 - 0xFC and whose second byte falls in
the range 0x40 - 0x7E or 0x80 - 0xFC.
This is summarized in the following diagram.
.millust begin
   [ 1st byte ]    [ 2nd byte ]
    0x81-0x9F       0x40-0xFC
        or          except 0x7F
    0xE0-0xFC
.millust end
.desc end
.return begin
The &func function returns zero if the argument is not in the range;
otherwise, a non-zero value is returned.
.return end
.see begin
.im seejto &function.
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>

JMOJI chars[] = {
    0x002C,
    0x8183,
    0x0035,
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
        if( jiszen( c ) ) {
          printf( "%c%c", c>>8, c );
          printf( " is a double-byte character" );
        } else {
          printf( "%c", c );
          printf( " is a single-byte character" );
        }
        printf( "\n" );
    }
  }
.exmp output
, is a single-byte character
 < is a double-byte character
5 is a single-byte character
 X is a double-byte character
.exmp end
.class WATCOM
.system
