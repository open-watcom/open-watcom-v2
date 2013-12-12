.func _mbctoupper
.synop begin
#include <mbstring.h>
unsigned int _mbctoupper( unsigned int c );
.ixfunc2 '&CharTest' &func
.synop end
.desc begin
The
.id &func.
function converts a lowercase multibyte character to an
equivalent uppercase multibyte character.
.np
For example, in code page 932, this includes the single-byte lowercase
letters a-z and the double-byte lowercase characters such that:
.millust begin
0x8281 <= c <= 0x829A
.millust end
.np
.bd Note:
This function was called
.kw jtoupper
in earlier versions.
.desc end
.return begin
The
.id &func.
function returns the argument value if the argument is not a
double-byte lowercase character;
otherwise, the equivalent uppercase character is returned.
.return end
.see begin
.im seembc &function.
.see end
.exmp begin
#include <stdio.h>
#include <mbctype.h>
#include <mbstring.h>

unsigned int chars[] = {
    'a',        /* single-byte a */
    'b',        /* single-byte b */
    'c',        /* single-byte c */
    'd',        /* single-byte d */
    'e',        /* single-byte e */
    0x8281,     /* double-byte a */
    0x8282,     /* double-byte b */
    0x8283,     /* double-byte c */
    0x8284,     /* double-byte d */
    0x8285      /* double-byte e */
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
      c = _mbctoupper( chars[ i ] );
      if( c > 0xff )
        printf( "%c%c", c>>8, c );
      else
        printf( "%c", c );
    }
    printf( "\n" );
  }
.exmp output
ABCDE A B C D E
.exmp end
.class WATCOM
.system
