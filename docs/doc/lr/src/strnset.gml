.func strnset _strnset _fstrnset _wcsnset _mbsnset _fmbsnset
.synop begin
#include <string.h>
char *strnset( char *str, int fill, size_t count );
.ixfunc2 '&String' &funcb
.if &'length(&_func.) ne 0 .do begin
char *_strnset( char *str, int fill, size_t count );
.ixfunc2 '&String' &_func
.do end
.if &farfnc eq 1 .do begin
char __far *_fstrnset( char __far *str,
                       int fill,
                       size_t count );
.ixfunc2 '&String' &ffunc
.do end
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
wchar_t *_wcsnset( wchar_t *str, int fill, size_t count );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&mfunc.) ne 0 .do begin
#include <mbstring.h>
unsigned char *_mbsnset( unsigned char *str,
                         unsigned int fill,
                         size_t count );
.ixfunc2 '&String' &mfunc
.ixfunc2 '&Multibyte' &mfunc
.do end
.if &'length(&fmfunc.) ne 0 .do begin
unsigned char __far *_fmbsnset( unsigned char __far *str,
                                unsigned int fill,
                                size_t __n );
.ixfunc2 '&String' &fmfunc
.ixfunc2 '&Multibyte' &fmfunc
.do end
.synop end
.desc begin
The
.id &funcb.
function fills the string
.arg str
with the value of the argument
.arg fill
.ct , converted to be a character value.
When the value of
.arg count
is greater than the length of the string, the entire string is filled.
Otherwise, that number of characters at the start of the string are set
to the fill character.
.im ansiconf
.im farfunc
.im widefunc
For &wfunc, the value of
.arg count
is the number of wide characters to fill.
This is half the number of bytes.
.im mbsffunc
.np
For &mfunc, the value of
.arg count
is the number of multibyte characters to fill.
If the number of bytes to be filled is odd and
.arg fill
is a double-byte character, the partial byte at the end is filled with
an ASCII space character.
.desc end
.return begin
The address of the original string
.arg str
is returned.
.return end
.see begin
.seelist strnset strset
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

char source[] = { "A sample STRING" };

void main()
  {
    printf( "%s\n", source );
    printf( "%s\n", strnset( source, '=', 100 ) );
    printf( "%s\n", strnset( source, '*', 7 ) );
  }
.exmp output
A sample STRING
===============
*******========
.exmp end
.class WATCOM
.system
