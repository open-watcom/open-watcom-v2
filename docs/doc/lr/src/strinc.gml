.func _strinc _wcsinc _mbsinc _fmbsinc
.synop begin
#include <tchar.h>
char *_strinc( const char *current );
.ixfunc2 '&String' &func
.if &'length(&wfunc.) ne 0 .do begin
wchar_t *_wcsinc( const wchar_t *current );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&mfunc.) ne 0 .do begin
#include <mbstring.h>
unsigned char *_mbsinc( const unsigned char *current );
.ixfunc2 '&String' &mfunc
.ixfunc2 '&Multibyte' &mfunc
.do end
.if &'length(&fmfunc.) ne 0 .do begin
unsigned char *_fmbsinc(
                    const unsigned char __far *current );
.ixfunc2 '&String' &fmfunc
.ixfunc2 '&Multibyte' &fmfunc
.do end
.synop end
.desc begin
The
.id &func.
function returns a pointer to the next character
(single-byte, wide, or multibyte) in the string pointed to by
.arg current
.ct .li .
You must ensure that
.arg current
does not point into the middle of a multibyte or wide character.
.im farparm
.im widefunc
.im mbsfunc
.desc end
.return begin
The
.id &func.
function returns a pointer to the next character
(single-byte, wide, or multibyte depending on the function used).
.return end
.see begin
.seelist &function. _strdec _strinc _strninc
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
