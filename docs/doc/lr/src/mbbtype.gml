.func _mbbtype
.synop begin
#include <mbstring.h>
#include <mbctype.h> (for manifest constants)
int _mbbtype( unsigned char ch, int type );
.synop end
.desc begin
The
.id &func.
function determines the type of a byte in a multibyte
character.
If the value of
.arg type
is any value except 1,
.id &func.
tests for a valid single-byte or lead
byte of a multibyte character.
If the value of
.arg type
is 1,
.id &func.
tests for a valid trail byte of a multibyte character.
.np
.us Note:
A similar function was called
.kw chkctype
in earlier versions.
.desc end
.return begin
If the value of
.arg type
is not 1, the
.id &func.
function returns one of the following values:
.begterm
.term _MBC_SINGLE
the character is a valid single-byte character
(e.g., 0x20 - 0x7E, 0xA1 - 0xDF in code page 932)
.term _MBC_LEAD
the character is valid lead byte character
(e.g., 0x81 - 0x9F, 0xE0 - 0xFC in code page 932)
.term _MBC_ILLEGAL
the character is an illegal character
(e.g., any value except 0x20 - 0x7E, 0xA1 - 0xDF, 0x81 - 0x9F,
0xE0 - 0xFC in code page 932)
.endterm
.np
If the value of
.arg type
is 1, the
.id &func.
function returns one of the following values:
.begterm
.term _MBC_TRAIL
the character is a valid trailing byte character
(e.g., 0x40 - 0x7E, 0x80 - 0xFC in code page 932)
.term _MBC_ILLEGAL
the character is an illegal character
(e.g., any character except a valid trailing byte character)
.endterm
.return end
.see begin
.im seeismbc &function.
.see end
.exmp begin
#include <stdio.h>
#include <mbctype.h>
#include <mbstring.h>

const char *types[4] = {
    "ILLEGAL",
    "SINGLE",
    "LEAD",
    "TRAIL"
};

const unsigned char chars[] = {
    ' ',
    '.',
    '1',
    'A',
    0x81,0x40, /* double-byte space */
    0x82,0x60, /* double-byte A */
    0x82,0xA6, /* double-byte Hiragana */
    0x83,0x42, /* double-byte Katakana */
    0xA1,      /* single-byte Katakana punctuation */
    0xA6,      /* single-byte Katakana alphabetic */
    0xDF,      /* single-byte Katakana alphabetic */
    0xE0,0xA1, /* double-byte Kanji */
    0x00
};

#define SIZE sizeof( chars ) / sizeof( unsigned char )

void main()
  {
    int     i, j, k;

    _setmbcp( 932 );
    k = 0;
    for( i = 0; i < SIZE; i++ ) {
      j = _mbbtype( chars[i], k );
      printf( "%s\n", types[ 1 + j ] );
      if( j == _MBC_LEAD )
        k = 1;
      else
        k = 0;
    }
  }
.exmp output
SINGLE
SINGLE
SINGLE
SINGLE
LEAD
TRAIL
LEAD
TRAIL
LEAD
TRAIL
LEAD
TRAIL
SINGLE
SINGLE
SINGLE
LEAD
TRAIL
ILLEGAL
.exmp end
.class WATCOM
.system
