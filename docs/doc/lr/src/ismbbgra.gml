.func _ismbbgraph
#include <mbctype.h>
int _ismbbgraph( unsigned int ch );
.synop end
.desc begin
The &func function tests if the argument
.arg ch
satisfies the condition that one of
.kw isgraph
or
.kw _ismbbkprint
is true.
.np
For example, in code page 932, &func tests if the argument
.arg ch
is a single-byte printable character excluding space ("&sysrb.") or
single-byte Katakana character.
.np
.us Note:
The argument
.arg ch
must represent a single-byte value (i.e., 0 <=
.arg ch
<= 255 ).
Incorrect results occur if the argument is a double-byte character.
This is shown by the example below.
.desc end
.return begin
The &func function returns a non-zero value if the argument satisfies
the condition; otherwise a zero value is returned.
.return end
.see begin
.im seeismbb &function.
.see end
.exmp begin
#include <stdio.h>
#include <mbctype.h>

unsigned int chars[] = {
    ' ',
    '.',
    '1',
    'A',
    0x8140, /* double-byte space */
    0x8260, /* double-byte A */
    0x82A6, /* double-byte Hiragana */
    0x8342, /* double-byte Katakana */
    0xA1,   /* single-byte Katakana punctuation */
    0xA6,   /* single-byte Katakana alphabetic */
    0xDF,   /* single-byte Katakana alphabetic */
    0xE0A1  /* double-byte Kanji */
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
      printf( "%#6.4x is %sa single-byte printable "
              "non-space character\n",
            chars[i],
            ( _ismbbgraph( chars[i] ) ) ? "" : "not " );
    }
  }
.exmp output
0x0020 is not a single-byte printable non-space character
0x002e is a single-byte printable non-space character
0x0031 is a single-byte printable non-space character
0x0041 is a single-byte printable non-space character
0x8140 is a single-byte printable non-space character
0x8260 is a single-byte printable non-space character
0x82a6 is a single-byte printable non-space character
0x8342 is a single-byte printable non-space character
0x00a1 is a single-byte printable non-space character
0x00a6 is a single-byte printable non-space character
0x00df is a single-byte printable non-space character
0xe0a1 is a single-byte printable non-space character
.exmp end
.class WATCOM
.system
