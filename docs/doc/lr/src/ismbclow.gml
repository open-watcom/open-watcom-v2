.func _ismbclower
.synop begin
#include <mbstring.h>
int _ismbclower( unsigned int ch );
.synop end
.desc begin
The
.id &func.
function tests for a valid lowercase multibyte character.
Multibyte characters include both single-byte and double-byte
characters.
For example, in code page 932, a lowercase double-byte character is
one for which the following expression is true:
.millust begin
0x8281 <= c <= 0x829A
.millust end
.desc end
.return begin
The
.id &func.
function returns a non-zero value when the argument is a
member of this set of characters; otherwise, zero is returned.
.return end
.see begin
.im seeismbc
.see end
.exmp begin
#include <stdio.h>
#include <mbctype.h>
#include <mbstring.h>
.exmp break
unsigned int chars[] = {
    '1',
    'A',
    'a',
    0x8140, /* double-byte space */
    0x8143, /* double-byte , */
    0x8254, /* double-byte 5 */
    0x8260, /* double-byte A */
    0x8279, /* double-byte Z */
    0x8281, /* double-byte a */
    0x829A, /* double-byte z */
    0x989F, /* double-byte L2 character */
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
            "multibyte lowercase character\n",
            chars[i],
            ( _ismbclower( chars[i] ) ) ? "" : "not " );
    }
  }
.exmp output
0x0031 is not a valid multibyte lowercase character
0x0041 is not a valid multibyte lowercase character
0x0061 is a valid multibyte lowercase character
0x8140 is not a valid multibyte lowercase character
0x8143 is not a valid multibyte lowercase character
0x8254 is not a valid multibyte lowercase character
0x8260 is not a valid multibyte lowercase character
0x8279 is not a valid multibyte lowercase character
0x8281 is a valid multibyte lowercase character
0x829a is a valid multibyte lowercase character
0x989f is not a valid multibyte lowercase character
0x00a6 is not a valid multibyte lowercase character
.exmp end
.class WATCOM
.system
