.func _mbterm _fmbterm
#include <mbstring.h>
int _mbterm( const unsigned char *ch );
.ixfunc2 '&Multibyte' &func
.if &farfnc eq 1 .do begin
int _fmbterm( const unsigned char __far *ch );
.ixfunc2 '&Multibyte' &ffunc
.do end
.funcend
.desc begin
The &func function determines if the next multibyte character in the
string pointed to by
.arg ch
is a null character or a valid lead byte followed by a null
character.
.im farparm
.desc end
.return begin
The &func function returns 1 if the multibyte character pointed to by
.arg ch
is a null character.
The &func function returns 2 if the multibyte character pointed to by
.arg ch
is a valid lead byte character followed by a null character.
Otherwise, the &func function returns 0.
.return end
.see begin
.im seembc &function.
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
    0x82,0x00  /* invalid double-byte */
};

#define SIZE sizeof( chars ) / sizeof( unsigned char )

void main()
{
    int     i;

    _setmbcp( 932 );
    for( i = 0; i < SIZE; i++ ) {
        printf( "0x%2.2x %d\n", chars[i],
                _mbterm( &chars[i] ) );
    }
}
.exmp output
0x20 0
0x2e 0
0x31 0
0x41 0
0x81 0
0x40 0
0x82 2
0x00 1
.exmp end
.class WATCOM
.system
