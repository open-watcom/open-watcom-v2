.func _strset _fstrset _wcsset _mbsset _fmbsset strset
.ansiname _strset
.synop begin
#include <string.h>
char *_strset( char *s1, int fill );
.ixfunc2 '&String' _strset
.if &farfnc ne 0 .do begin
char __far *_fstrset( char __far *s1, int fill );
.ixfunc2 '&String' _fstrset
.do end
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
wchar_t *_wcsset( wchar_t *s1, int fill );
.ixfunc2 '&String' _wcsset
.ixfunc2 '&Wide' _wcsset
.do end
.if &'length(&mfunc.) ne 0 .do begin
#include <mbstring.h>
unsigned char *_mbsset( unsigned char *s1,
                        unsigned int fill );
.ixfunc2 '&String' _mbsset
.ixfunc2 '&Multibyte' _mbsset
.do end
.if &'length(&fmfunc.) ne 0 .do begin
unsigned char __far *_fmbsset( unsigned char __far *s1,
                               unsigned int fill );
.ixfunc2 '&String' _fmbsset
.ixfunc2 '&Multibyte' _fmbsset
.do end

.deprec
char *strset( char *s1, int fill );
.ixfunc2 '&String' strset
.synop end
.desc begin
The
.id &funcb.
function fills the string pointed to by
.arg s1
with the character
.arg fill
.period
The terminating null character in the original string remains
unchanged.
.farfunc &ffunc. &funcb.
.widefunc &wfunc. &funcb.
.mbcsfunc &mfunc. &funcb.
.farfunc &fmfunc. &mfunc.
.np
.deprfunc strset _strset
.desc end
.return begin
The address of the original string
.arg s1
is returned.
.return end
.see begin
.seelist _strnset
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

char source[] = { "A sample STRING" };

void main()
  {
    printf( "%s\n", source );
    printf( "%s\n", _strset( source, '=' ) );
    printf( "%s\n", _strset( source, '*' ) );
  }
.exmp output
A sample STRING
===============
***************
.exmp end
.class WATCOM
.system
