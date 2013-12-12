.func strset _strset _fstrset _wcsset _mbsset _fmbsset
#include <string.h>
char *strset( char *s1, int fill );
.ixfunc2 '&String' &func
.if &'length(&_func.) ne 0 .do begin
char *_strset( char *s1, int fill );
.ixfunc2 '&String' &_func
.do end
.if &farfnc eq 1 .do begin
char __far *_fstrset( char __far *s1, int fill );
.ixfunc2 '&String' &ffunc
.do end
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
wchar_t *_wcsset( wchar_t *s1, int fill );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&mfunc.) ne 0 .do begin
#include <mbstring.h>
unsigned char *_mbsset( unsigned char *s1,
                        unsigned int fill );
.ixfunc2 '&String' &mfunc
.ixfunc2 '&Multibyte' &mfunc
.do end
.if &'length(&fmfunc.) ne 0 .do begin
unsigned char __far *_fmbsset( unsigned char __far *s1,
                               unsigned int fill );
.ixfunc2 '&String' &fmfunc
.ixfunc2 '&Multibyte' &fmfunc
.do end
.synop end
.desc begin
The &func function fills the string pointed to by
.arg s1
with the character
.arg fill
.ct .li .
The terminating null character in the original string remains
unchanged.
.if &'length(&_func.) ne 0 .do begin
.np
The &_func function is identical to &func..
Use &_func for ANSI naming conventions.
.do end
.im farfunc
.im widefunc
.im mbsffunc
.desc end
.return begin
The address of the original string
.arg s1
is returned.
.return end
.see begin
.seelist strset strnset
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

char source[] = { "A sample STRING" };

void main()
  {
    printf( "%s\n", source );
    printf( "%s\n", strset( source, '=' ) );
    printf( "%s\n", strset( source, '*' ) );
  }
.exmp output
A sample STRING
===============
***************
.exmp end
.class WATCOM
.system
