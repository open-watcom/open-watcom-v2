.func wctomb _fwctomb
#include <stdlib.h>
int wctomb( char *s, wchar_t wc );
.ixfunc2 '&Wide' &func
.ixfunc2 '&Multibyte' &func
.if &farfnc eq 1 .do begin
#include <mbstring.h>
int _fwctomb( char __far *s, wchar_t wc );
.ixfunc2 '&Wide' &ffunc
.ixfunc2 '&Multibyte' &ffunc
.do end
.funcend
.*
.safealt
.*
.desc begin
The &func function determines the number of bytes required to
represent the multibyte character corresponding to the wide character
contained in
.arg wc
.ct .li .
If
.arg s
is not a NULL pointer, the multibyte character representation is
stored in the array pointed to by
.arg s
.ct .li .
At most
.kw MB_CUR_MAX
characters will be stored.
.im farparm
.desc end
.return begin
If
.arg s
is a NULL pointer, the &func function returns zero if multibyte
character encodings are not state dependent, and non-zero otherwise.
If
.arg s
is not a NULL pointer, the &func function returns:
.begnote $setptnt 6
.termhd1 Value
.termhd2 Meaning
.note &minus.1
if the value of
.arg wc
does not correspond to a valid multibyte character
.note len
the number of bytes that comprise the multibyte character
corresponding to the value of
.arg wc
.ct .li .
.endnote
.return end
.see begin
.seelist wctomb wctomb_s mblen mbstowcs mbstowcs_s mbtowc wcstombs wcstombs_s
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>

wchar_t wchar = { 0x0073 };
char    mbbuffer[2];
.exmp break
void main()
  {
    int len;
.exmp break
    printf( "Character encodings are %sstate dependent\n",
            ( wctomb( NULL, 0 ) )
            ? "" : "not " );

    len = wctomb( mbbuffer, wchar );
    mbbuffer[len] = '\0';
    printf( "%s(%d)\n", mbbuffer, len );
  }
.exmp output
Character encodings are not state dependent
s(1)
.exmp end
.class ANSI
.system
