.func _mbctolower
.synop begin
#include <mbstring.h>
unsigned int _mbctolower( unsigned int c );
.ixfunc2 '&CharTest' &func
.synop end
.desc begin
The
.id &func.
function converts an uppercase multibyte character to an
equivalent lowercase multibyte character.
.np
For example, in code page 932, this includes the single-byte uppercase
letters A-Z and the double-byte uppercase characters such that:
.millust begin
0x8260 <= c <= 0x8279
.millust end
.np
.us Note:
This function was called
.kw jtolower
in earlier versions.
.desc end
.return begin
The
.id &func.
function returns the argument value if the argument is not a
double-byte uppercase character;
otherwise, the equivalent lowercase character is returned.
.return end
.see begin
.im seembc &function.
.see end
.exmp begin
#include <stdio.h>
#include <mbctype.h>
#include <mbstring.h>

unsigned int chars[] = {
    'A',        /* single-byte A */
    'B',        /* single-byte B */
    'C',        /* single-byte C */
    'D',        /* single-byte D */
    'E',        /* single-byte E */
    0x8260,     /* double-byte A */
    0x8261,     /* double-byte B */
    0x8262,     /* double-byte C */
    0x8263,     /* double-byte D */
    0x8264      /* double-byte E */
};
.exmp break
#define SIZE sizeof( chars ) / sizeof( unsigned int )
.exmp break
void main()
  {
    int   i;
    unsigned int c;
.exmp break
    _setmbcp( 932 );
    for( i = 0; i < SIZE; i++ ) {
      c = _mbctolower( chars[ i ] );
      if( c > 0xff )
        printf( "%c%c", c>>8, c );
      else
        printf( "%c", c );
    }
    printf( "\n" );
  }
.exmp output
abcde a b c d e
.exmp end
.class WATCOM
.system
