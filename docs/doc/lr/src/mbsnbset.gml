.func _mbsnbset _fmbsnbset
.synop begin
#include <mbstring.h>
unsigned char *_mbsnbset( unsigned char *str,
                          unsigned int fill,
                          size_t count );
.ixfunc2 '&String' &func
.if &farfnc eq 1 .do begin
unsigned char __far *_fmbsnbset( unsigned char __far *str,
                                 unsigned int fill,
                                 size_t count );
.ixfunc2 '&String' &ffunc
.do end
.synop end
.desc begin
The &func function fills the string
.arg str
with the value of the argument
.arg fill
.ct .li .
When the value of
.arg len
is greater than the length of the string, the entire string is filled.
Otherwise, that number of characters at the start of the string are set
to the fill character.
.np
&func is similar to
.kw _mbsnset
.ct , except that it fills in
.arg count
bytes rather than
.arg count
characters.
If the number of bytes to be filled is odd and
.arg fill
is a double-byte character, the partial byte at the end is filled with
an ASCII space character.
.im farfunc
.desc end
.return begin
The address of the original string
.arg str
is returned.
.return end
.see begin
.seelist &function. _mbsnbset strnset strset
.see end
.exmp begin
#include <stdio.h>
#include <string.h>
#include <mbctype.h>
#include <mbstring.h>

void main()
  {
    unsigned char   big_string[10];
    int             i;

    _setmbcp( 932 );
    memset( (char *) big_string, 0xee, 10 );
    big_string[9] = 0x00;
    for( i = 0; i < 10; i++ )
        printf( "%2.2x ", big_string[i] );
    printf( "\n" );
    _mbsnbset( big_string, 0x8145, 5 );
    for( i = 0; i < 10; i++ )
        printf( "%2.2x ", big_string[i] );
    printf( "\n" );

  }
.exmp output
ee ee ee ee ee ee ee ee ee 00
81 45 81 45 20 ee ee ee ee 00
.exmp end
.class WATCOM
.system
