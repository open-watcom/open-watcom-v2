.func strdup _strdup _fstrdup wcsdup _wcsdup _mbsdup _fmbsdup
.synop begin
#include <string.h>
char *strdup( const char *src );
.ixfunc2 '&String' strdup
.ixfunc2 '&Copy' strdup
.if &'length(&_func.) ne 0 .do begin
char *_strdup( const char *src );
.ixfunc2 '&String' _strdup
.ixfunc2 '&Copy' _strdup
.do end
.if &farfnc ne 0 .do begin
char __far *_fstrdup( const char __far *src );
.ixfunc2 '&String' _fstrdup
.ixfunc2 '&Copy' _fstrdup
.do end
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
wchar_t *wcsdup( const wchar_t *src );
.ixfunc2 '&String' wcsdup
.ixfunc2 '&Copy' wcsdup
.ixfunc2 '&Wide' wcsdup
wchar_t *_wcsdup( const wchar_t *src );
.ixfunc2 '&String' _wcsdup
.ixfunc2 '&Copy' _wcsdup
.ixfunc2 '&Wide' _wcsdup
.do end
.if &'length(&mfunc.) ne 0 .do begin
#include <mbstring.h>
unsigned char *_mbsdup( unsigned char *src );
.ixfunc2 '&String' _mbsdup
.ixfunc2 '&Copy' _mbsdup
.ixfunc2 '&Multibyte' _mbsdup
.do end
.if &'length(&fmfunc.) ne 0 .do begin
unsigned char __far *_fmbsdup( unsigned char __far *src );
.ixfunc2 '&String' _fmbsdup
.ixfunc2 '&Copy' _fmbsdup
.ixfunc2 '&Multibyte' _fmbsdup
.do end
.synop end
.desc begin
The
.id &funcb.
function creates a duplicate copy of the string pointed to
by
.arg src
and returns a pointer to the new copy.
.if &farfnc eq 0 .do begin
The memory for the new string is obtained by using the
.reffunc malloc
function
and can be freed using the
.reffunc free
function.
.do end
.el .do begin
For
.id &funcb.
the memory for the new string is obtained by using the
.reffunc malloc
function and can be freed using the
.reffunc free
function.
For &ffunc, the memory for the new string is obtained by using the
.reffunc _fmalloc
function and can be freed using the
.reffunc _ffree
function.
.do end
.im ansiconf
.farfunc &ffunc. &funcb.
.widefunc &wfunc. &funcb.
.widefunc _&wfunc. &funcb.
.mbcsfunc &mfunc. &funcb.
.farfunc &fmfunc. &mfunc.
.desc end
.return begin
The
.id &funcb.
function returns the pointer to the new copy of the string
if successful, otherwise it
returns
.mono NULL.
.return end
.see begin
.seelist strdup free malloc strcpy strncpy
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

void main()
  {
    char *dup;
.exmp break
    dup = strdup( "Make a copy" );
    printf( "%s\n", dup );
  }
.exmp end
.class WATCOM
.system
