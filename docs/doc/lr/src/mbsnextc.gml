.func _mbsnextc _fmbsnextc _strnextc _wcsnextc
.synop begin
#include <mbstring.h>
unsigned int _mbsnextc( const unsigned char *s );
.ixfunc2 '&String' _mbsnextc
.ixfunc2 '&Multibyte' _mbsnextc
.if &farfnc ne 0 .do begin
unsigned int _fmbsnextc( const unsigned char __far *s );
.ixfunc2 '&String' _fmbsnextc
.ixfunc2 '&Multibyte' _fmbsnextc
.do end
#include <tchar.h>
unsigned int _strnextc( const char *s );
.ixfunc2 '&String' _strnextc
unsigned int _wcsnextc( const wchar_t *s ) {
.ixfunc2 '&String' _wcsnextc
.ixfunc2 '&Wide' _wcsnextc
.synop end
.desc begin
The
.id &funcb.
function returns the integer value of the next multi-byte
character in
.arg s
.ct , without advancing the string pointer.
.id &funcb.
recognizes multi-byte character sequences according to the
multi-byte character code page currently in use.
.farfuncp &ffunc. &funcb.
.tcsfunc _tcsnextc &funcb. _strnextc &wfunc.
.desc end
.return begin
These functions return the integer value of the next character
(multi-byte, wide, or single-byte) pointed to by
.arg s
.period
.return end
.see begin
.seelist _mbsnextc _mbsdec _mbsinc _mbsninc
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
    _setmbcp( 932 );
    printf( "%#6.4x\n", _mbsnextc( &chars[2] ) );
    printf( "%#6.4x\n", _mbsnextc( &chars[4] ) );
    printf( "%#6.4x\n", _mbsnextc( &chars[12] ) );
  }
.exmp output
0x0031
0x8140
0x00a1
.exmp end
.class WATCOM
.system
