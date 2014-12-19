.func strupr _strupr _fstrupr _wcsupr _mbsupr _fmbsupr
.synop begin
#include <string.h>
char *strupr( char *s );
.ixfunc2 '&String' &funcb
.if &'length(&_func.) ne 0 .do begin
char *_strupr( char *s );
.ixfunc2 '&String' &_func
.do end
.if &farfnc eq 1 .do begin
char __far *_fstrupr( char __far *s );
.ixfunc2 '&String' &ffunc
.do end
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
wchar_t *_wcsupr( wchar_t *s );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&mfunc.) ne 0 .do begin
#include <mbstring.h>
unsigned char *_mbsupr( unsigned char *s );
.ixfunc2 '&String' &mfunc
.ixfunc2 '&Multibyte' &mfunc
.do end
.if &'length(&fmfunc.) ne 0 .do begin
unsigned char __far *_fmbsupr( unsigned char __far *s );
.ixfunc2 '&String' &fmfunc
.ixfunc2 '&Multibyte' &fmfunc
.do end
.synop end
.desc begin
The
.id &funcb.
function replaces the string
.arg s
with uppercase characters by invoking the
.kw toupper
function for each character in the string.
.if &'length(&_func.) ne 0 .do begin
.np
The
.id &_func.
function is identical to &funcb..
Use
.id &_func.
for ISO C naming conventions.
.do end
.im farfunc
.im widefunc
.im mbsfunc
.desc end
.return begin
The address of the original string
.arg s
is returned.
.return end
.see begin
.seelist strupr strlwr
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

char source[] = { "A mixed-case STRING" };

void main()
  {
    printf( "%s\n", source );
    printf( "%s\n", strupr( source ) );
    printf( "%s\n", source );
  }
.exmp output
A mixed-case STRING
A MIXED-CASE STRING
A MIXED-CASE STRING
.exmp end
.ansiname &_func
.class WATCOM
.system
