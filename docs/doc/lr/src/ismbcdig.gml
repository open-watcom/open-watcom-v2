.func _ismbcdigit
#include <mbstring.h>
int _ismbcdigit( unsigned int ch );
.synop end
.desc begin
The &func function tests for any multibyte decimal-digit character '0'
through '9'.
In code page 932, this includes the corresponding double-byte versions
of these characters.
.desc end
.return begin
The &func function returns a non-zero value when the argument is a
decimal-digit character.
Otherwise, zero is returned.
.return end
.see begin
.im seeismbc &function.
.see end
.exmp begin
#include <stdio.h>
#include <mbctype.h>
#include <mbstring.h>
.exmp break
unsigned int chars[] = {
    '.',
    '1',
    'A',
    0x8143, /* double-byte , */
    0x8183, /* double-byte < */
    0x8254, /* double-byte 5 */
    0x8277, /* double-byte X */
    0xA6
};
.exmp break
#define SIZE sizeof( chars ) / sizeof( unsigned int )
.exmp break
void main()
  {
    int   i;
.exmp break
    _setmbcp( 932 );
    for( i = 0; i < SIZE; i++ ) {
      printf( "%#6.4x is %sa valid "
            "multibyte digit character\n",
            chars[i],
            ( _ismbcdigit( chars[i] ) ) ? "" : "not " );
    }
  }
.exmp output
0x002e is not a valid multibyte digit character
0x0031 is a valid multibyte digit character
0x0041 is not a valid multibyte digit character
0x8143 is not a valid multibyte digit character
0x8183 is not a valid multibyte digit character
0x8254 is a valid multibyte digit character
0x8277 is not a valid multibyte digit character
0x00a6 is not a valid multibyte digit character
.exmp end
.class WATCOM
.system
