.func _mbctombb
#include <mbstring.h>
unsigned int _mbctombb( unsigned int ch );
.funcend
.desc begin
The &func function returns the single-byte character equivalent
to the double-byte character
.arg ch
.ct .li .
The single-byte character will be in the range 0x20 through 0x7E or
0xA1 through 0xDF.
.np
.us Note:
This function was called
.kw zentohan
in earlier versions.
.desc end
.return begin
The &func function returns
.arg ch
if there is no equivalent single-byte character;
otherwise &func returns a single-byte character.
.return end
.see begin
.im seeismbb &function.
.see end
.exmp begin
#include <stdio.h>
#include <mbctype.h>
#include <mbstring.h>

#define ZEN(x) 130*256+(x-1+32)

unsigned int alphabet[26] = {
    ZEN('A'),ZEN('B'),ZEN('C'),ZEN('D'),ZEN('E'),
    ZEN('F'),ZEN('G'),ZEN('H'),ZEN('I'),ZEN('J'),
    ZEN('K'),ZEN('L'),ZEN('M'),ZEN('N'),ZEN('O'),
    ZEN('P'),ZEN('Q'),ZEN('R'),ZEN('S'),ZEN('T'),
    ZEN('U'),ZEN('V'),ZEN('W'),ZEN('X'),ZEN('Y'),
    ZEN('Z')
};

#define SIZE sizeof( alphabet ) / sizeof( unsigned int )

void main()
  {
    int             i;
    unsigned int    c;

    _setmbcp( 932 );
    for( i = 0; i < SIZE; i++ ) {
      c = _mbctombb( alphabet[ i ] );
      printf( "%c", c );
    }
    printf( "\n" );
  }
.exmp output
ABCDEFGHIJKLMNOPQRSTUVWXYZ
.exmp end
.class WATCOM
.system
