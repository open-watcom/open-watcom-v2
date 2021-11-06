.func _mbsninc _fmbsninc _strninc _wcsninc
.synop begin
#include <mbstring.h>
unsigned char *_mbsninc( const unsigned char *s, size_t count );
.ixfunc2 '&String' _mbsninc
.ixfunc2 '&Multibyte' _mbsninc
.if &farfnc ne 0 .do begin
unsigned char __far *_fmbsninc( const unsigned char __far *s,
                                size_t count );
.ixfunc2 '&String' _fmbsninc
.ixfunc2 '&Multibyte' _fmbsninc
.do end
#include <tchar.h>
char *_strninc( const char *s, size_t count );
.ixfunc2 '&String' _strninc
.if &'length(&wfunc.) ne 0 .do begin
wchar_t *_wcsninc( const wchar_t *s, size_t count );
.ixfunc2 '&String' _wcsninc
.ixfunc2 '&Wide' _wcsninc
.do end
.synop end
.desc begin
The
.id &funcb.
function increments
.arg s
by
.arg count
of multi-byte characters.
.id &funcb.
recognizes multi-byte character sequences according to the
multi-byte character code page currently in use.
.farfunc &ffunc. &funcb.
.tcsfunc _tcsninc &funcb. _strninc &wfunc.
.desc end
.return begin
These functions return a pointer to
.arg s
after it has been incremented by
.arg count
of characters (multi-byte, wide, or single-byte) or
.kw NULL
if
.arg s
was
.kw NULL
.period
If
.arg count
exceeds the number of characters remaining in the string, the result is
undefined.
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
      next = _mbsninc( next, 1 );
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
