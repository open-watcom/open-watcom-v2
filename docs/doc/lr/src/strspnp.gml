.func strspnp _strspnp _fstrspnp _wcsspnp _mbsspnp _fmbsspnp
.synop begin
#include <string.h>
char *strspnp( const char *str,
               const char *charset );
.ixfunc2 '&String' &funcb
.ixfunc2 '&Search' &funcb
.if &'length(&_func.) ne 0 .do begin
char *_strspnp( const char *str,
                const char *charset );
.ixfunc2 '&String' &_func
.ixfunc2 '&Search' &_func
.do end
.if &farfnc eq 1 .do begin
char __far *_fstrspnp( const char __far *str,
                       const char __far *charset );
.ixfunc2 '&String' &ffunc
.ixfunc2 '&Search' &ffunc
.do end
.if &'length(&wfunc.) ne 0 .do begin
#include <tchar.h>
wchar_t *_wcsspnp( const wchar_t *str,
                   const wchar_t *charset );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Search' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&mfunc.) ne 0 .do begin
#include <mbstring.h>
unsigned char *_mbsspnp( const unsigned char *str,
                         const unsigned char *charset );
.ixfunc2 '&String' &mfunc
.ixfunc2 '&Search' &mfunc
.ixfunc2 '&Multibyte' &mfunc
.do end
.if &'length(&fmfunc.) ne 0 .do begin
unsigned char __far *_fmbsspnp(
                    const unsigned char __far *str,
                    const unsigned char __far *charset );
.ixfunc2 '&String' &fmfunc
.ixfunc2 '&Search' &fmfunc
.ixfunc2 '&Multibyte' &fmfunc
.do end
.synop end
.desc begin
The
.id &funcb.
function returns a pointer to the first character in
.arg str
that does not belong to the set of characters in
.arg charset
.ct .li .
The terminating null character is not considered to be part of
.arg charset
.ct .li .
.if &'length(&_func.) ne 0 .do begin
.np
The
.id &_func.
function is identical to &funcb..
Use
.id &_func.
for ANSI/ISO naming conventions.
.do end
.im farparm
.im widefunc
.im mbsffunc
.desc end
.return begin
The
.id &funcb.
function returns
.mono NULL
if
.arg str
consists entirely of characters from
.arg charset
.ct .li .
.return end
.see begin
.seelist strspnp strcspn strspn
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

void main()
  {
    printf( "%s\n", strspnp( "out to lunch", "aeiou" ) );
    printf( "%s\n", strspnp( "out to lunch", "xyz" ) );
  }
.exmp output
t to lunch
out to lunch
.exmp end
.class begin WATCOM
.ansiname &_func
.class end
.system
