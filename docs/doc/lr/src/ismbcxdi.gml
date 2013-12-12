.func _ismbcxdigit
.synop begin
#include <mbstring.h>
int _ismbcxdigit( unsigned int ch );
.synop end
.desc begin
The
.id &func.
function tests for any multibyte hexadecimal-digit character
'0' through '9' or 'A' through 'F'.
In code page 932, this includes the corresponding double-byte versions
of these characters.
.desc end
.return begin
The
.id &func.
function returns a non-zero value when the argument is a
hexadecimal-digit character.
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
    0x8143, /* double-byte "," */
    0x8183, /* double-byte "<" */
    0x8254, /* double-byte "5" */
    0x8265, /* double-byte "F" */
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
            "multibyte hexadecimal digit character\n",
            chars[i],
            ( _ismbcxdigit( chars[i] ) ) ? "" : "not " );
    }
  }
.exmp output
0x002e is not a valid multibyte hexadecimal digit character
0x0031 is a valid multibyte hexadecimal digit character
0x0041 is a valid multibyte hexadecimal digit character
0x8143 is not a valid multibyte hexadecimal digit character
0x8183 is not a valid multibyte hexadecimal digit character
0x8254 is a valid multibyte hexadecimal digit character
0x8265 is a valid multibyte hexadecimal digit character
0x00a6 is not a valid multibyte hexadecimal digit character
.exmp end
.class WATCOM
.system
