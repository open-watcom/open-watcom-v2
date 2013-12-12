.func strrev _strrev _fstrrev _wcsrev _mbsrev _fmbsrev
.synop begin
#include <string.h>
char *strrev( char *s1 );
.ixfunc2 '&String' &func
.if &'length(&_func.) ne 0 .do begin
char *_strrev( char *s1 );
.ixfunc2 '&String' &_func
.do end
.if &farfnc eq 1 .do begin
char __far *_fstrrev( char __far *s1 );
.ixfunc2 '&String' &ffunc
.do end
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
wchar_t *_wcsrev( wchar_t *s1 );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&mfunc.) ne 0 .do begin
#include <mbstring.h>
unsigned char *_mbsrev( unsigned char *s1 );
.ixfunc2 '&String' &mfunc
.ixfunc2 '&Multibyte' &mfunc
.do end
.if &'length(&fmfunc.) ne 0 .do begin
unsigned char __far *_fmbsrev( unsigned char __far *s1 );
.ixfunc2 '&String' &fmfunc
.ixfunc2 '&Multibyte' &fmfunc
.do end
.synop end
.desc begin
The
.id &func.
function replaces the string
.arg s1
with a string whose characters are in the reverse order.
.if &'length(&_func.) ne 0 .do begin
.np
The
.id &_func.
function is identical to &func..
Use
.id &_func.
for ANSI/ISO naming conventions.
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
.exmp begin
#include <stdio.h>
#include <string.h>

char source[] = { "A sample STRING" };

void main()
  {
    printf( "%s\n", source );
    printf( "%s\n", strrev( source ) );
    printf( "%s\n", strrev( source ) );
  }
.exmp output
A sample STRING
GNIRTS elpmas A
A sample STRING
.exmp end
.class begin WATCOM
.ansiname &_func
.class end
.system
