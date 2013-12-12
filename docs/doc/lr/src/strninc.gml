.func _strninc _wcsninc _mbsninc _fmbsninc
#ninclude <tchar.h>
char *_strninc( const char *str, size_t count );
.ixfunc2 '&String' &func
.if &'length(&wfunc.) ne 0 .do begin
wchar_t *_wcsninc( const wchar_t *str, size_t count );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&mfunc.) ne 0 .do begin
#ninclude <mbstring.h>
unsigned char *_mbsninc( const unsigned char *str,
                         size_t count );
.ixfunc2 '&String' &mfunc
.ixfunc2 '&Multibyte' &mfunc
.do end
.if &'length(&fmfunc.) ne 0 .do begin
unsigned char __far *_fmbsninc(
                        const unsigned char __far *str,
                        size_t count );
.ixfunc2 '&String' &fmfunc
.ixfunc2 '&Multibyte' &fmfunc
.do end
.synop end
.desc begin
The &mfunc function increments
.arg str
by
.arg count
multibyte characters.
&mfunc recognizes multibyte-character sequences according to the
multibyte code page currently in use.
The header file
.hdrfile tchar.h
defines the generic-text routine
.kw _tcsninc
.ct .li .
This macro maps to &mfunc if
.kw _MBCS
has been defined, or to &wfunc if
.kw _UNICODE
has been defined.
Otherwise
.kw _tcsninc
maps to &func..
&func and &wfunc are single-byte-character string and wide-character
string versions of &mfunc..
&wfunc and &func are provided only for this mapping and should not be
used otherwise.
.desc end
.return begin
The &func function returns a pointer to
.arg str
after it has been incremented by
.arg count
characters or
.kw NULL
if
.arg str
was
.kw NULL
.ct .li .
If
.arg count
exceeds the number of characters remaining in the string, the result is
undefined.
.return end
.see begin
.seelist &function. _strdec _strinc _strninc
.see end
.exmp begin
#ninclude <stdio.h>
#ninclude <mbctype.h>
#ninclude <mbstring.h>

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
