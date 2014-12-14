.func _ismbcgraph
.synop begin
#include <mbstring.h>
int _ismbcgraph( unsigned int ch );
.synop end
.desc begin
The
.id &func.
function tests for any printable multibyte character except
space ('&sysrb').
The
.kw _ismbcprint
function is similar, except that the space character is also included
in the character set being tested.
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
    '.',
    ' ',
    '1',
    'A',
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
            "multibyte graph character\n",
            chars[i],
            ( _ismbcgraph( chars[i] ) ) ? "" : "not " );
    }
  }
.exmp output
0x002e is a valid multibyte graph character
0x0020 is not a valid multibyte graph character
0x0031 is a valid multibyte graph character
0x0041 is a valid multibyte graph character
0x8140 is not a valid multibyte graph character
0x8143 is a valid multibyte graph character
0x8254 is a valid multibyte graph character
0x8260 is a valid multibyte graph character
0x8279 is a valid multibyte graph character
0x8281 is a valid multibyte graph character
0x829a is a valid multibyte graph character
0x989f is a valid multibyte graph character
0x00a6 is a valid multibyte graph character
.exmp end
.class WATCOM
.system
