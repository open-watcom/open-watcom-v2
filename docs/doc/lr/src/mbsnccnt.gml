.func _mbsnccnt _fmbsnccnt _strncnt _wcsncnt
.synop begin
.sr func=_strncnt
#include <mbstring.h>
size_t _mbsnccnt( const unsigned char *string, size_t n );
.ixfunc2 '&String' _mbsnccnt
.ixfunc2 '&Multibyte' _mbsnccnt
.if &farfnc ne 0 .do begin
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
function counts the number of multibyte characters in the
first
.arg n
bytes of the string
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
.farfuncp &ffunc. &funcb.
.np
The header file
.hdrfile tchar.h
defines the generic-text routine
.kw _tcsnccnt
.period
This macro maps to
.id &funcb.
if
.kw _MBCS
has been defined, or to the
.id &wfunc.
macro if
.kw _UNICODE
has been defined.
Otherwise
.kw _tcsnccnt
maps to _strncnt.
.id _strncnt
and
.id &wfunc.
are single-byte character string and wide-character
string versions of &funcb.
.period
The
.id _strncnt
and
.id &wfunc.
macros are provided only for this mapping and
should not be used otherwise.
.np
The
.id &funcb.
function returns the number of characters (i.e.,
.arg n
.ct )
in the first
.arg n
bytes of the single-byte string
.arg string
.period
The
.id &wfunc.
function returns the number of bytes (i.e., 2 *
.arg n
.ct )
in the first
.arg n
wide characters of the wide-character string
.arg string
.period
.desc end
.return begin
.id &funcb.
returns the number of characters from the beginning
of the string to byte
.arg n
.period
.id &wfunc.
returns the number of wide characters from the beginning
of the string to byte
.arg n
.period
.id &funcb.
returns the number of multibyte characters from the beginning
of the string to byte
.arg n
.period
If these functions find a null character before byte
.arg n
.ct , they return the number of characters before the null character.
If the string consists of fewer than
.arg n
characters, these functions return the number of characters in the
string.
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
    printf( "%d characters found\n",
            _mbsnccnt( chars, 10 ) );
  }
.exmp output
7 characters found
.exmp end
.class WATCOM
.system
