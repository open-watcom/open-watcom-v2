.func wcrtomb _fwcrtomb
.synop begin
#include <wchar.h>
int wcrtomb( char *s, wchar_t wc, mbstate_t *ps );
.ixfunc2 '&Wide' &func
.ixfunc2 '&Multibyte' &func
.if &farfnc eq 1 .do begin
int _fwcrtomb( char __far *s, wchar_t wc, mbstate_t __far *ps );
.ixfunc2 '&Wide' &ffunc
.ixfunc2 '&Multibyte' &ffunc
.do end
.synop end
.*
.safealt
.*
.desc begin
If
.arg s
is a null pointer, the &func function determines the number of bytes
necessary to enter the initial shift state (zero if encodings are not
state-dependent or if the initial conversion state is described).
The resulting state described will be the initial conversion state.
.np
If
.arg s
is not a null pointer, the &func function determines the number of
bytes needed to represent the multibyte character that corresponds
to the wide character given by
.arg wc
(including any shift sequences), and stores the resulting bytes in the
array whose first element is pointed to by
.arg s
.ct .li .
At most
.kw MB_CUR_MAX
bytes will be stored.
If
.arg wc
is a null wide character, the resulting state described will be the
initial conversion state.
.im farparm
.im mbcrstrt
.desc end
.return begin
If
.arg s
is a null pointer, the &func function returns the number of bytes
necessary to enter the initial shift state.
The value returned will not be greater than that of the
.kw MB_CUR_MAX
macro.
.np
If
.arg s
is not a null pointer, the &func function returns the number of bytes
stored in the array object (including any shift sequences) when
.arg wc
is a valid wide character; otherwise (when
.arg wc
is not a valid wide character), an encoding error occurs, the value
of the macro
.kw EILSEQ
will be stored in
.kw errno
and &minus.1 will be returned, but the conversion state will be
unchanged.
.return end
.see begin
.im seembc &function.
.see end
.exmp begin
#include <stdio.h>
#include <wchar.h>
#include <mbctype.h>
#include <errno.h>

const wchar_t wc[] = {
    0x0020,
    0x002e,
    0x0031,
    0x0041,
    0x3000,     /* double-byte space */
    0xff21,     /* double-byte A */
    0x3048,     /* double-byte Hiragana */
    0x30a3,     /* double-byte Katakana */
    0xff61,     /* single-byte Katakana punctuation */
    0xff66,     /* single-byte Katakana alphabetic */
    0xff9f,     /* single-byte Katakana alphabetic */
    0x720d,     /* double-byte Kanji */
    0x0000
};
.exmp break
#define SIZE sizeof( wc ) / sizeof( wchar_t )
.exmp break
void main()
{
    int         i, j, k;
    char        s[2];

    _setmbcp( 932 );
    i = wcrtomb( NULL, 0, NULL );
    printf( "Number of bytes to enter "
            "initial shift state = %d\n", i );
    j = 1;
    for( i = 0; i < SIZE; i++ ) {
        j = wcrtomb( s, wc[i], NULL );
        printf( "%d bytes in character ", j );
        if( errno == EILSEQ ) {
          printf( " - illegal wide character\n" );
        } else {
          if ( j == 0 ) {
              k = 0;
          } else if ( j == 1 ) {
              k = s[0];
          } else if( j == 2 ) {
              k = s[0]<<8 | s[1];
          }
          printf( "(%#6.4x->%#6.4x)\n", wc[i], k );
        }
    }
}
.exmp output
Number of bytes to enter initial shift state = 0
1 bytes in character (0x0020->0x0020)
1 bytes in character (0x002e->0x002e)
1 bytes in character (0x0031->0x0031)
1 bytes in character (0x0041->0x0041)
2 bytes in character (0x3000->0x8140)
2 bytes in character (0xff21->0x8260)
2 bytes in character (0x3048->0x82a6)
2 bytes in character (0x30a3->0x8342)
1 bytes in character (0xff61->0x00a1)
1 bytes in character (0xff66->0x00a6)
1 bytes in character (0xff9f->0x00df)
2 bytes in character (0x720d->0xe0a1)
1 bytes in character (  0000->0x0069)
.exmp end
.class ANSI
.system
