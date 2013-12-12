.func mbtowc _fmbtowc
.synop begin
#include <stdlib.h>
int mbtowc( wchar_t *pwc, const char *s, size_t n );
.ixfunc2 '&Multibyte' &func
.if &farfnc eq 1 .do begin
#include <mbstring.h>
int _fmbtowc( wchar_t __far *pwc,
              const char __far *s,
              size_t n );
.ixfunc2 '&Multibyte' &ffunc
.do end
.synop end
.desc begin
The &func function converts a single multibyte character pointed to by
.arg s
into the wide character code that corresponds to that multibyte
character.
The code for the null character is zero.
If the multibyte character is valid and
.arg pwc
is not a NULL pointer, the code is stored in the object pointed to by
.arg pwc
.ct .li .
At most
.arg n
bytes of the array pointed to by
.arg s
will be examined.
.np
The &func function does not examine more than
.kw MB_CUR_MAX
bytes.
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
.note 0
if
.arg s
points to the null character
.note len
the number of bytes that comprise the multibyte character (if the next
.arg n
or fewer bytes form a valid multibyte character)
.note &minus.1
if the next
.arg n
bytes do not form a valid multibyte character
.endnote
.return end
.see begin
.seelist mbtowc mblen wctomb mbstowcs wcstombs
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>
#include <mbctype.h>

void main()
  {
    char    *wc = "string";
    wchar_t wbuffer[10];
    int     i, len;
.exmp break
    _setmbcp( 932 );
    printf( "Character encodings are %sstate dependent\n",
            ( mbtowc( wbuffer, NULL, 0 ) )
            ? "" : "not " );

    len = mbtowc( wbuffer, wc, MB_CUR_MAX );
    wbuffer[len] = '\0';
    printf( "%s(%d)\n", wc, len );
    for( i = 0; i < len; i++ )
        printf( "/%4.4x", wbuffer[i] );
    printf( "\n" );
  }
.exmp output
Character encodings are not state dependent
string(1)
/0073
.exmp end
.class ANSI
.system
