.func strdup _strdup _fstrdup _wcsdup _mbsdup _fmbsdup
.synop begin
#include <string.h>
char *strdup( const char *src );
.ixfunc2 '&String' &funcb
.ixfunc2 '&Copy' &funcb
.if &'length(&_func.) ne 0 .do begin
char *_strdup( const char *src );
.ixfunc2 '&String' &_func
.ixfunc2 '&Copy' &_func
.do end
.if &farfnc eq 1 .do begin
char __far *_fstrdup( const char __far *src );
.ixfunc2 '&String' &ffunc
.ixfunc2 '&Copy' &ffunc
.do end
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
wchar_t *_wcsdup( const wchar_t *src );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Copy' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&mfunc.) ne 0 .do begin
#include <mbstring.h>
unsigned char *_mbsdup( unsigned char *src );
.ixfunc2 '&String' &mfunc
.ixfunc2 '&Copy' &mfunc
.ixfunc2 '&Multibyte' &mfunc
.do end
.if &'length(&fmfunc.) ne 0 .do begin
unsigned char __far *_fmbsdup( unsigned char __far *src );
.ixfunc2 '&String' &fmfunc
.ixfunc2 '&Copy' &fmfunc
.ixfunc2 '&Multibyte' &fmfunc
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
.kw malloc
function
and can be freed using the
.kw free
function.
.do end
.el .do begin
For
.id &funcb.
the memory for the new string is obtained by using the
.kw malloc
function and can be freed using the
.kw free
function.
For &ffunc, the memory for the new string is obtained by using the
.kw _fmalloc
function and can be freed using the
.kw _ffree
function.
.do end
.im ansiconf
.im farparm
.im widefunc
.im mbsffunc
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
