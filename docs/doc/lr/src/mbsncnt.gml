.func _mbsnbcnt _fmbsnbcnt _mbsnccnt _fmbsnccnt _strncnt _wcsncnt
.synop begin
#include <mbstring.h>
size_t _mbsnbcnt( const unsigned char *string, size_t n );
.ixfunc2 '&String' _mbsnbcnt
.ixfunc2 '&Multibyte' _mbsnbcnt
size_t _mbsnccnt( const unsigned char *string, size_t n );
.ixfunc2 '&String' _mbsnccnt
.ixfunc2 '&Multibyte' _mbsnccnt
.if &farfnc ne 0 .do begin
size_t _fmbsnbcnt( const unsigned char __far *string, size_t n );
.ixfunc2 '&String' _fmbsnbcnt
.ixfunc2 '&Multibyte' _fmbsnbcnt
size_t _fmbsnccnt( const unsigned char __far *string, size_t n );
.ixfunc2 '&String' _fmbsnccnt
.ixfunc2 '&Multibyte' _fmbsnccnt
.do end
#include <tchar.h>
size_t _strncnt( const char *string, size_t n );
.ixfunc2 '&String' _strncnt
.if &'length(&wfunc.) ne 0 .do begin
size_t _wcsncnt( const wchar_t *string, size_t n ) {
.ixfunc2 '&String' _wcsncnt
.ixfunc2 '&Wide' _wcsncnt
.do end
.synop end
.desc begin
The
.id &funcb.
function counts the number of bytes in the first
.arg n
multibyte characters of the string
.arg string
.period
.cp
The
.id _mbsnccnt
function counts the number of multibyte characters in the first
.arg n
bytes of the string
.arg string
.period
If
.id _mbsnccnt
finds a null byte as the second byte of a double-byte
character, the first (lead) byte is not included in the count.
.cp
.us Note:
.id _mbsnbcnt
function was called
.kw mtob
and
.id _mbsnccnt
function was called
.kw btom
in earlier versions.
.cp
.if &farfnc ne 0 .do begin
.np
The
.id _fmbsnbcnt
function is a data model independent form of the _mbsnbcnt
function that accepts far pointer arguments.
.cp
The
.id _fmbsnccnt
function is a data model independent form of the _fmbsnccnt
function that accepts far pointer arguments.
It is most useful in mixed memory model applications.
.do end
.np
.farfuncp &ffunc. &funcb.
.br
.farfuncp &fmfunc. &mfunc.
.np
.tcshdr _tcsnbcnt _tcsnccnt
.tcsbody _tcsnbcnt &funcb. _strncnt &wfunc.
.tcsbody _tcsnccnt &mfunc. _strncnt &wfunc.
.tcsfoot _strncnt &wfunc.
.desc end
.return begin
The
.id _mbsnbcnt
functions return the number of bytes in the string from the beginning
of the string up to the specified number of characters
.arg n
.period
.np
The
.id _mbsnccnt
funstions return the number of characters in the string from the beginning
of the string up to the specified number of bytes
.arg n
or until a null character is encountered.
The null character is not included in the count.
If the character preceding the null character was a lead byte, the lead
byte is not included in the count.
.return end
.see begin
.seelist _mbsnbcat
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
    printf( "%d characters found\n",
            _mbsnccnt( chars, 10 ) );
  }
.exmp output
14 bytes found
7 characters found
.exmp end
.class WATCOM
.system
