.func _mbsnbcat _fmbsnbcat
.synop begin
#include <mbstring.h>
unsigned char *_mbsnbcat( unsigned char *dst,
                    const unsigned char *src,
                                 size_t n );
.ixfunc2 '&String' &func
.ixfunc2 '&Concats' &func
.if &farfnc eq 1 .do begin
unsigned char __far *_fmbsnbcat( unsigned char __far *dst,
                           const unsigned char __far *src,
                                              size_t n );
.ixfunc2 '&String' &ffunc
.ixfunc2 '&Concats' &ffunc
.do end
.synop end
.desc begin
The &func function appends not more than
.arg n
bytes of the string pointed to by
.arg src
to the end of the string pointed to by
.arg dst
.ct .li .
If the byte immediately preceding the null character in
.arg dst
is a lead byte, the initial byte of
.arg src
overwrites this lead byte.
Otherwise, the initial byte of
.arg src
overwrites the terminating null character at the end of
.arg dst
.ct .li .
If the last byte to be copied from
.arg src
is a lead byte, the lead byte is not copied and a null character
replaces it in
.arg dst
.ct .li .
In any case, a terminating null character is always appended to the
result.
.im farfunc
.desc end
.return begin
The &func function returns the value of
.arg dst
.ct .li .
.return end
.see begin
.seelist &function. _mbsnbcat _mbsnbcmp _mbsnbcpy _mbsnbicnp _mbsnbset
.seelist &function. _mbsnccnt strncat strcat
.see end
.exmp begin
#include <stdio.h>
#include <string.h>
#include <mbctype.h>
#include <mbstring.h>

const unsigned char str1[] = {
    0x81,0x40, /* double-byte space */
    0x82,0x60, /* double-byte A */
    0x00
};

const unsigned char str2[] = {
    0x81,0x40, /* double-byte space */
    0x82,0xA6, /* double-byte Hiragana */
    0x83,0x42, /* double-byte Katakana */
    0x00
};

void main()
  {
    unsigned char   big_string[10];
    int             i;

    _setmbcp( 932 );
    memset( (char *) big_string, 0xee, 10 );
    big_string[9] = 0x00;
    printf( "Length of string = %d\n",
            strlen( (char *) big_string ) );
    for( i = 0; i < 10; i++ )
        printf( "%2.2x ", big_string[i] );
    printf( "\n" );
.exmp break
    _mbsnset( big_string, 0x8145, 5 );
    for( i = 0; i < 10; i++ )
        printf( "%2.2x ", big_string[i] );
    printf( "\n" );
.exmp break
    big_string[0] = 0x00;
    _mbsnbcat( big_string, str1, 3 );
    for( i = 0; i < 10; i++ )
        printf( "%2.2x ", big_string[i] );
    printf( "\n" );
.exmp break
    big_string[2] = 0x84;
    big_string[3] = 0x00;
    for( i = 0; i < 10; i++ )
        printf( "%2.2x ", big_string[i] );
    printf( "\n" );
.exmp break
    _mbsnbcat( big_string, str2, 5 );
    for( i = 0; i < 10; i++ )
        printf( "%2.2x ", big_string[i] );
    printf( "\n" );

  }
.exmp output
Length of string = 9
ee ee ee ee ee ee ee ee ee 00
81 45 81 45 81 45 81 45 20 00
81 40 00 00 81 45 81 45 20 00
81 40 84 00 81 45 81 45 20 00
81 40 81 40 82 a6 00 00 20 00
.exmp end
.class WATCOM
.system
