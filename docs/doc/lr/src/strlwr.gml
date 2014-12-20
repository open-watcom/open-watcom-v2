.func strlwr _strlwr _fstrlwr _wcslwr _mbslwr _fmbslwr
.synop begin
#include <string.h>
char *strlwr( char *s1 );
.ixfunc2 '&String' &funcb
.if &'length(&_func.) ne 0 .do begin
char *_strlwr( char *s1 );
.ixfunc2 '&String' &_func
.do end
.if &farfnc eq 1 .do begin
char __far *_fstrlwr( char __far *s1 );
.ixfunc2 '&String' &ffunc
.do end
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
wchar_t *_wcslwr( wchar_t *s1 );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&mfunc.) ne 0 .do begin
#include <mbstring.h>
unsigned char *_mbslwr( unsigned char *s1 );
.ixfunc2 '&String' &mfunc
.ixfunc2 '&Multibyte' &mfunc
.do end
.if &'length(&fmfunc.) ne 0 .do begin
unsigned char __far *_fmbslwr( unsigned char __far *s1 );
.ixfunc2 '&String' &fmfunc
.ixfunc2 '&Multibyte' &fmfunc
.do end
.synop end
.desc begin
The
.id &funcb.
function replaces the string
.arg s1
with lowercase characters by invoking the
.kw tolower
function for each character in the string.
.im ansiconf
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
.seelist strlwr strupr
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

char source[] = { "A mixed-case STRING" };

void main()
  {
    printf( "%s\n", source );
    printf( "%s\n", strlwr( source ) );
    printf( "%s\n", source );
  }
.exmp output
A mixed-case STRING
a mixed-case string
a mixed-case string
.exmp end
.class WATCOM
.system
