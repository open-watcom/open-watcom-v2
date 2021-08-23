.func _mbsnccnt _fmbsnccnt _strncnt _wcsncnt
.synop begin
#include <mbstring.h>
size_t _mbsnccnt( const unsigned char *string, size_t n );
.ixfunc2 '&String' _mbsnccnt
.ixfunc2 '&Compare' _mbsnccnt
.if &farfnc ne 0 .do begin
size_t _fmbsnccnt( const unsigned char __far *string, size_t n );
.ixfunc2 '&String' _fmbsnccnt
.ixfunc2 '&Compare' _fmbsnccnt
.do end
#include <tchar.h>
size_t _strncnt( const char *string, size_t n );
.ixfunc2 '&String' _strncnt
.ixfunc2 '&Compare' _strncnt
.if &'length(&wfunc.) ne 0 .do begin
size_t _wcsncnt( const wchar_t *string, size_t n );
.ixfunc2 '&String' _wcsncnt
.ixfunc2 '&Compare' _wcsncnt
.do end
.synop end
.desc begin
The
.id &funcb.
function counts the number of characters in the first
.arg n
bytes of
.arg string
.period
.np
The
.id &funcb.
function counts the number of multibyte characters in the
first
.arg n
bytes of
.arg string
.period
If
.id &funcb.
finds a null byte as the second byte of a double-byte
character, the first (lead) byte is not included in the count.
.np
.us Note:
This function was called
.kw btom
in earlier versions.
.farfunc &ffunc. &funcb.
.tcsfunc _tcsnccnt &funcb. _strncnt &wfunc.
.desc end
.return begin
These functions return the number of characters
(single-byte, wide, or multi-byte) from
the beginning of the string to byte
.arg n
.period
If these functions find a null character before byte
.arg n
.ct , they return the number of characters before the null character.
If the string consists of fewer than
.arg n
characters, these functions return the number of characters in the
string.
If
.arg n
is less than zero, these functions return 0.
.return end
.see begin
.seelist strcmp stricmp strncmp strnicmp strcmpi strcasecmp strncasecmp
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
    printf( "%d characters found\n", _mbsnccnt( chars, 10 ) );
  }
.exmp output
7 characters found
.exmp end
.class WATCOM
.system
