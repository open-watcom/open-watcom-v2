.func _mbsnbcnt _fmbsnbcnt _strncnt _wcsncnt
.synop begin
#include <mbstring.h>
size_t _mbsnbcnt( const unsigned char *s, size_t n );
.ixfunc2 '&String' _mbsnbcnt
.ixfunc2 '&Multibyte' _mbsnbcnt
.if &farfnc ne 0 .do begin
size_t _fmbsnbcnt( const unsigned char __far *s, size_t n );
.ixfunc2 '&String' _fmbsnbcnt
.ixfunc2 '&Multibyte' _fmbsnbcnt
.do end
#include <tchar.h>
size_t _strncnt( const char *s, size_t n );
.ixfunc2 '&String' _strncnt
.if &'length(&wfunc.) ne 0 .do begin
size_t _wcsncnt( const wchar_t *s, size_t n );
.ixfunc2 '&String' _wcsncnt
.ixfunc2 '&Wide' _wcsncnt
.do end
.synop end
.desc begin
The
.id &funcb.
function counts the number of bytes in the first
.arg n
multi-byte characters of the string
.arg s
.period
.np
.us Note:
This function was called
.kw mtob
in earlier versions.
.farfuncp &ffunc. &funcb.
.tcsfunc _tcsnbcnt &funcb. _strncnt &wfunc.
.desc end
.return begin
These functions return the number of bytes in the string up to the
specified number of characters (multi-byte, wide, or single-byte)
or until a null character is encountered.
The null character is not included in the count.
If the character preceding the null character was a lead byte, the lead
byte is not included in the count.
.return end
.see begin
.seelist _mbsnbcat _mbsnbcnt _mbsnccnt
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
void main()
  {
    _setmbcp( 932 );
    printf( "%d bytes found\n",
            _mbsnbcnt( chars, 10 ) );
  }
.exmp output
14 bytes found
.exmp end
.class WATCOM
.system
