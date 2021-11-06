.func _mbsinc _fmbsinc _strinc _wcsinc
.synop begin
#include <mbstring.h>
unsigned char *_mbsinc( const unsigned char *current );
.ixfunc2 '&String' _mbsinc
.ixfunc2 '&Multibyte' _mbsinc
.if &farfnc ne 0 .do begin
unsigned char __far *_fmbsinc( const unsigned char __far *current );
.ixfunc2 '&String' _fmbsinc
.ixfunc2 '&Multibyte' _fmbsinc
.do end
#include <tchar.h>
char *_strinc( const char *current );
.ixfunc2 '&String' _strinc
wchar_t *_wcsinc( const wchar_t *current );
.ixfunc2 '&String' _wcsinc
.ixfunc2 '&Wide' _wcsinc
.synop end
.desc begin
The
.id &funcb.
function returns a pointer to the next multi-byte character
in the string pointed to by
.arg current
.period
You must ensure that
.arg current
does not point into the middle of a multi-byte or wide character.
.farfunc &ffunc. &funcb.
.tcsfunc _tcsinc &funcb. _strinc &wfunc.
.desc end
.return begin
These functions return a pointer to the next character
(multi-byte, wide, or single-byte).
.return end
.see begin
.seelist _mbsdec _mbsinc _mbsninc
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

#define SIZE sizeof( chars ) / sizeof( unsigned char )

void main()
  {
    int                 j, k;
    const unsigned char *next;

    _setmbcp( 932 );
    next = chars;
    do {
      next = _mbsinc( next );
      j = mblen( next, MB_CUR_MAX );
      if( j == 0 ) {
        k = 0;
      } else if ( j == 1 ) {
        k = *next;
      } else if( j == 2 ) {
        k = *(next)<<8 | *(next+1);
      }
      printf( "Next character %#6.4x\n", k );
    } while( next != &chars[ SIZE - 1 ] );
  }
.exmp output
Next character 0x002e
Next character 0x0031
Next character 0x0041
Next character 0x8140
Next character 0x8260
Next character 0x82a6
Next character 0x8342
Next character 0x00a1
Next character 0x00a6
Next character 0x00df
Next character 0xe0a1
Next character   0000
.exmp end
.class WATCOM
.system
