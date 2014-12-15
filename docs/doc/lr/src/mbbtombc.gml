.func _mbbtombc
.synop begin
#include <mbstring.h>
unsigned int _mbbtombc( unsigned int ch );
.synop end
.desc begin
The
.id &funcb.
function returns the double-byte character equivalent to the
single-byte character
.arg ch
.ct .li .
The single-byte character must be in the range 0x20 through 0x7E or
0xA1 through 0xDF.
.np
.us Note:
This function was called
.kw hantozen
in earlier versions.
.desc end
.return begin
The
.id &funcb.
function returns
.arg ch
if there is no equivalent double-byte character;
otherwise
.id &funcb.
returns a double-byte character.
.return end
.see begin
.im seeismbb
.see end
.exmp begin
#include <stdio.h>
#include <mbctype.h>
#include <mbstring.h>

char alphabet[] = {
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
};

void main()
  {
    int             i;
    unsigned short  c;

    _setmbcp( 932 );
    for( i = 0; i < sizeof( alphabet ) - 1; i++ ) {
      c = _mbbtombc( alphabet[ i ] );
      printf( "%c%c", c>>8, c );
    }
    printf( "\n" );
  }
.exmp output
A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
.exmp end
.class WATCOM
.system
