.func _strupr _fstrupr _wcsupr _mbsupr _fmbsupr strupr
.ansiname _strupr
.synop begin
#include <string.h>
char *_strupr( char *s );
.ixfunc2 '&String' _strupr
.if &farfnc ne 0 .do begin
char __far *_fstrupr( char __far *s );
.ixfunc2 '&String' _fstrupr
.do end
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
wchar_t *_wcsupr( wchar_t *s );
.ixfunc2 '&String' _wcsupr
.ixfunc2 '&Wide' _wcsupr
.do end
.if &'length(&mfunc.) ne 0 .do begin
#include <mbstring.h>
unsigned char *_mbsupr( unsigned char *s );
.ixfunc2 '&String' _mbsupr
.ixfunc2 '&Multibyte' _mbsupr
.do end
.if &'length(&fmfunc.) ne 0 .do begin
unsigned char __far *_fmbsupr( unsigned char __far *s );
.ixfunc2 '&String' _fmbsupr
.ixfunc2 '&Multibyte' _fmbsupr
.do end

.deprec
char *strupr( char *s );
.ixfunc2 '&String' strupr
.synop end
.desc begin
The
.id &funcb.
function replaces the string
.arg s
with uppercase characters by invoking the
.reffunc toupper
function for each character in the string.
.farfunc &ffunc. &funcb.
.widefunc &wfunc. &funcb.
.mbcsfunc &mfunc. &funcb.
.farfunc &fmfunc. &mfunc.
.np
.deprfunc strupr _strupr
.desc end
.return begin
The address of the original string
.arg s
is returned.
.return end
.see begin
.seelist _strupr _strlwr
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

char source[] = { "A mixed-case STRING" };

void main()
  {
    printf( "%s\n", source );
    printf( "%s\n", _strupr( source ) );
    printf( "%s\n", source );
  }
.exmp output
A mixed-case STRING
A MIXED-CASE STRING
A MIXED-CASE STRING
.exmp end
.class WATCOM
.system
