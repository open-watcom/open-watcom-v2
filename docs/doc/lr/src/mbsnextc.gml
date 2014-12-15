.func _mbsnextc _fmbsnextc _strnextc _wcsnextc
.synop begin
.sr func=_strnextc
#include <mbstring.h>
unsigned int _mbsnextc( const unsigned char *string );
.ixfunc2 '&String' &mfunc
.ixfunc2 '&Multibyte' &mfunc
.if &farfnc eq 1 .do begin
unsigned int _fmbsnextc(
                    const unsigned char __far *string );
.ixfunc2 '&String' &fmfunc
.ixfunc2 '&Multibyte' &fmfunc
.do end
#include <tchar.h>
unsigned int _strnextc( const char *string );
.ixfunc2 '&String' &funcb
unsigned int _wcsnextc( const wchar_t *string ) {
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Wide' &wfunc
.synop end
.desc begin
The
.id &mfunc.
function returns the integer value of the next
multibyte-character in
.arg string
.ct , without advancing the string pointer.
.id &mfunc.
recognizes multibyte character sequences according to the
multibyte code page currently in use.
.np
The header file
.hdrfile tchar.h
defines the generic-text routine
.kw _tcsnextc
.ct .li .
This macro maps to
.id &mfunc.
if
.kw _MBCS
has been defined, or to
.id &wfunc.
if
.kw _UNICODE
has been defined.
Otherwise
.kw _tcsnextc
maps to &funcb..
.id &funcb.
and
.id &wfunc.
are single-byte character string and wide-character
string versions of &mfunc..
.id &funcb.
and
.id &wfunc.
are provided only for this mapping and should not be
used otherwise.
.id &funcb.
returns the integer value of the next single-byte character in
the string.
.id &wfunc.
returns the integer value of the next wide character in the
string.
.desc end
.return begin
These functions return the integer value of the next character
(single-byte, wide, or multibyte) pointed to by
.arg string
.ct .li .
.return end
.see begin
.seelist _mbsnextc _strdec _strinc _strninc
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
