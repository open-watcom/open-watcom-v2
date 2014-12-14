.func _mbvtop _fmbvtop
.synop begin
#include <mbstring.h>
unsigned char *_mbvtop( unsigned int ch,
                        unsigned char *addr );
.ixfunc2 '&Multibyte' &func
.if &farfnc eq 1 .do begin
unsigned char __far *_fmbvtop( unsigned int ch,
                        unsigned char __far *addr );
.ixfunc2 '&Multibyte' &ffunc
.do end
.synop end
.desc begin
The
.id &func.
function stores the multibyte character
.arg ch
into the string pointed to by
.arg addr
.ct .li .
.im farparm
.desc end
.return begin
The
.id &func.
function returns the value of the argument
.arg addr
.ct .li .
.return end
.see begin
.im seembc
.see end
.exmp begin
#include <stdio.h>
#include <mbctype.h>
#include <mbstring.h>


void main()
  {
    unsigned char string[10];
    unsigned char *p;
    int           i;

    _setmbcp( 932 );
    p = string;
    _mbvtop( '.', p );
    p++;
    _mbvtop( '1', p );
    p++;
    _mbvtop( 'A', p );
    p++;
    _mbvtop( 0x8140, p );
    p += 2;
    _mbvtop( 0x8260, p );
    p += 2;
    _mbvtop( 0x82A6, p );
    p += 2;
    _mbvtop( '\0', p );

    for( i = 0; i < 10; i++ )
      printf( "%2.2x ", string[i] );
    printf( "\n" );
  }
.exmp output
2e 31 41 81 40 82 60 82 a6 00
.exmp end
.class WATCOM
.system
