.func _mbsnbcpy _fmbsnbcpy
.synop begin
#include <mbstring.h>
unsigned char *_mbsnbcpy( unsigned char *dst,
                    const unsigned char *src,
                    size_t n );
.ixfunc2 '&String' &func
.ixfunc2 '&Copy' &func
.if &farfnc eq 1 .do begin
unsigned char __far *_fmbsnbcpy( unsigned char __far *dst,
                           const unsigned char __far *src,
                           size_t n );
.ixfunc2 '&String' &ffunc
.ixfunc2 '&Copy' &ffunc
.do end
.synop end
.desc begin
The
.id &func.
function copies no more than
.arg n
bytes from the string pointed to by
.arg src
into the array pointed to by
.arg dst
.ct .li .
Copying of overlapping objects is not guaranteed to work properly.
.np
If the string pointed to by
.arg src
is shorter than
.arg n
bytes, null characters are appended to the copy in the array pointed
to by
.arg dst
.ct , until
.arg n
bytes in all have been written.
If the string pointed to by
.arg src
is longer than
.arg n
characters, then the result will not be terminated by a null character.
.im farfunc
.desc end
.return begin
The
.id &func.
function returns the value of
.arg dst
.ct .li .
.return end
.see begin
.seelist _mbsnbcpy strcpy strdup
.see end
.exmp begin
#include <stdio.h>
#include <mbctype.h>
#include <mbstring.h>

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

void main()
  {
    unsigned char   chars2[20];
    int             i;

    _setmbcp( 932 );
    _mbsnset( chars2, 0xFF, 20 );
    _mbsnbcpy( chars2, chars, 11 );
    for( i = 0; i < 20; i++ )
        printf( "%2.2x ", chars2[i] );
    printf( "\n" );
    _mbsnbcpy( chars2, chars, 20 );
    for( i = 0; i < 20; i++ )
        printf( "%2.2x ", chars2[i] );
    printf( "\n" );
  }
.exmp output
20 2e 31 41 81 40 82 60 82 a6 83 ff ff ff ff ff ff ff ff ff
20 2e 31 41 81 40 82 60 82 a6 83 42 a1 a6 df e0 a1 00 00 00
.exmp end
.class WATCOM
.system
