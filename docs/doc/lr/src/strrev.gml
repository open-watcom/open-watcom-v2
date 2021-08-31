.func _strrev _fstrrev _wcsrev _mbsrev _fmbsrev strrev
.synop begin
#include <string.h>
char *_strrev( char *s1 );
.ixfunc2 '&String' _strrev
.if &farfnc ne 0 .do begin
char __far *_fstrrev( char __far *s1 );
.ixfunc2 '&String' _fstrrev
.do end
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
wchar_t *_wcsrev( wchar_t *s1 );
.ixfunc2 '&String' _wcsrev
.ixfunc2 '&Wide' _wcsrev
.do end
.if &'length(&mfunc.) ne 0 .do begin
#include <mbstring.h>
unsigned char *_mbsrev( unsigned char *s1 );
.ixfunc2 '&String' _mbsrev
.ixfunc2 '&Multibyte' _mbsrev
.do end
.if &'length(&fmfunc.) ne 0 .do begin
unsigned char __far *_fmbsrev( unsigned char __far *s1 );
.ixfunc2 '&String' _fmbsrev
.ixfunc2 '&Multibyte' _fmbsrev
.do end

.deprec
char *strrev( char *s1 );
.ixfunc2 '&String' strrev
.synop end
.desc begin
The
.id &funcb.
function replaces the string
.arg s1
with a string whose characters are in the reverse order.
.im ansiconf
.farfunc &ffunc. &funcb.
.widefunc &wfunc. &funcb.
.mbcsfunc &mfunc. &funcb.
.farfunc &fmfunc. &mfunc.
.np
.deprfunc strrev _strrev
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
    printf( "%s\n", _strrev( source ) );
    printf( "%s\n", _strrev( source ) );
  }
.exmp output
A sample STRING
GNIRTS elpmas A
A sample STRING
.exmp end
.class WATCOM
.system
