.func _strdec _wcsdec _mbsdec _fmbsdec
.synop begin
#include <tchar.h>
char *_strdec( const char *start, const char *current );
.ixfunc2 '&String' &func
.if &'length(&wfunc.) ne 0 .do begin
wchar_t *_wcsdec( const wchar_t *start,
                  const wchar_t *current );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&mfunc.) ne 0 .do begin
#include <mbstring.h>
unsigned char *_mbsdec( const unsigned char *start,
                        const unsigned char *current );
.ixfunc2 '&String' &mfunc
.ixfunc2 '&Multibyte' &mfunc
.do end
.if &'length(&fmfunc.) ne 0 .do begin
unsigned char *_fmbsdec( const unsigned char __far *start,
                     const unsigned char __far *current );
.ixfunc2 '&String' &fmfunc
.ixfunc2 '&Multibyte' &fmfunc
.do end
.synop end
.desc begin
The
.id &func.
function returns a pointer to the previous character
(single-byte, wide, or multibyte) in the string pointed to by
.arg start
which must precede
.arg current
.ct .li .
The current character in the string is pointed to by
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
function returns a pointer to the previous character
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
.exmp break
#define SIZE sizeof( chars ) / sizeof( unsigned char )
.exmp break
void main()
  {
    int                 j, k;
    const unsigned char *prev;

    _setmbcp( 932 );
    prev = &chars[ SIZE - 1 ];
    do {
      prev = _mbsdec( chars, prev );
      j = mblen( prev, MB_CUR_MAX );
      if( j == 0 ) {
        k = 0;
      } else if ( j == 1 ) {
        k = *prev;
      } else if( j == 2 ) {
        k = *(prev)<<8 | *(prev+1);
      }
      printf( "Previous character %#6.4x\n", k );
    } while( prev != chars );
  }
.exmp output
Previous character 0xe0a1
Previous character 0x00df
Previous character 0x00a6
Previous character 0x00a1
Previous character 0x8342
Previous character 0x82a6
Previous character 0x8260
Previous character 0x8140
Previous character 0x0041
Previous character 0x0031
Previous character 0x002e
Previous character 0x0020
.exmp end
.class WATCOM
.system
