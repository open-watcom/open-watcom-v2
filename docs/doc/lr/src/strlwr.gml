.func _strlwr _fstrlwr _wcslwr _mbslwr _fmbslwr strlwr
.ansiname _strlwr
.synop begin
#include <string.h>
char *_strlwr( char *s1 );
.ixfunc2 '&String' &_func
.if &farfnc ne 0 .do begin
char __far *_fstrlwr( char __far *s1 );
.ixfunc2 '&String' _strlwr
.do end
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
wchar_t *_wcslwr( wchar_t *s1 );
.ixfunc2 '&String' _wcslwr
.ixfunc2 '&Wide' _wcslwr
.do end
.if &'length(&mfunc.) ne 0 .do begin
#include <mbstring.h>
unsigned char *_mbslwr( unsigned char *s1 );
.ixfunc2 '&String' _mbslwr
.ixfunc2 '&Multibyte' _mbslwr
.do end
.if &'length(&fmfunc.) ne 0 .do begin
unsigned char __far *_fmbslwr( unsigned char __far *s1 );
.ixfunc2 '&String' _fmbslwr
.ixfunc2 '&Multibyte' _fmbslwr
.do end

.deprec
char *strlwr( char *s1 );
.ixfunc2 '&String' strlwr
.synop end
.desc begin
The
.id &funcb.
function replaces the string
.arg s1
with lowercase characters by invoking the
.reffunc tolower
function for each character in the string.
.farfunc &ffunc. &funcb.
.widefunc &wfunc. &funcb.
.mbcsfunc &mfunc. &funcb.
.farfunc &fmfunc. &mfunc.
.np
.deprfunc strlwr _strlwr
.desc end
.return begin
The address of the original string
.arg s1
is returned.
.return end
.see begin
.seelist _strlwr _strupr
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

char source[] = { "A mixed-case STRING" };

void main()
  {
    printf( "%s\n", source );
    printf( "%s\n", _strlwr( source ) );
    printf( "%s\n", source );
  }
.exmp output
A mixed-case STRING
a mixed-case string
a mixed-case string
.exmp end
.class WATCOM
.system
