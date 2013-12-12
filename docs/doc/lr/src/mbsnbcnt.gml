.func _mbsnbcnt _fmbsnbcnt _strncnt _wcsncnt
.synop begin
.sr func=_strncnt
#include <mbstring.h>
size_t _mbsnbcnt( const unsigned char *string, size_t n );
.ixfunc2 '&String' &mfunc
.ixfunc2 '&Multibyte' &mfunc
.if &farfnc eq 1 .do begin
size_t _fmbsnbcnt( const unsigned char __far *string,
                   size_t n );
.ixfunc2 '&String' &fmfunc
.ixfunc2 '&Multibyte' &fmfunc
.do end
#include <tchar.h>
size_t _strncnt( const char *string, size_t n );
.ixfunc2 '&String' &func
.if &'length(&wfunc.) ne 0 .do begin
size_t _wcsncnt( const wchar_t *string, size_t n ) {
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The
.id &mfunc.
function counts the number of bytes in the first
.arg n
multibyte characters of the string
.arg string
.ct .li .
.np
.us Note:
This function was called
.kw mtob
in earlier versions.
.if &farfnc eq 1 .do begin
.np
The
.id &fmfunc.
function is a data model independent form of the &func
function that accepts far pointer arguments.
It is most useful in mixed memory model applications.
.do end
.np
The header file
.hdrfile tchar.h
defines the generic-text routine
.kw _tcsnbcnt
.ct .li .
This macro maps to
.id &mfunc.
if
.kw _MBCS
has been defined, or to the
.id &wfunc.
macro if
.kw _UNICODE
has been defined.
Otherwise
.kw _tcsnbcnt
maps to &func..
.id &func.
and
.id &wfunc.
are single-byte character string and wide-character
string versions of &mfunc..
The
.id &func.
and
.id &wfunc.
macros are provided only for this mapping and
should not be used otherwise.
.np
The
.id &func.
function returns the number of characters (i.e.,
.arg n
.ct )
in the first
.arg n
bytes of the single-byte string
.arg string
.ct .li .
The
.id &wfunc.
function returns the number of bytes (i.e., 2 *
.arg n
.ct )
in the first
.arg n
wide characters of the wide-character string
.arg string
.ct .li .
.desc end
.return begin
The
.id &func.
functions return the number of bytes in the string up to the
specified number of characters or until a null character is
encountered.
The null character is not included in the count.
If the character preceding the null character was a lead byte, the lead
byte is not included in the count.
.return end
.see begin
.seelist &function. _mbsnbcat _mbsnbcnt _mbsnccnt
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
