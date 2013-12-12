.func _strnicoll _wcsnicoll _mbsnicoll
.synop begin
#include <string.h>
int _strnicoll( const char *s1,
                const char *s2,
                size_t count );
.ixfunc2 '&String' &func
.ixfunc2 '&Compare' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
int _wcsnicoll( const wchar_t *s1,
                const wchar_t *s2,
                size_t count );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Compare' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&mfunc.) ne 0 .do begin
#include <mbstring.h>
int _mbsnicoll( const unsigned char *s1,
                const unsigned char *s2,
                size_t count );
.ixfunc2 '&String' &mfunc
.ixfunc2 '&Compare' &mfunc
.ixfunc2 '&Multibyte' &mfunc
.do end
.synop end
.desc begin
These functions perform a case insensitive comparison of the first
.arg count
characters of the string pointed to by
.arg s1
to the string pointed to by
.arg s2
.ct .li .
The comparison uses the current code page which can be selected by the
.kw _setmbcp
function.
.im widefunc
.im mbsfunc
.desc end
.return begin
These functions return an integer less than, equal to, or greater than
zero, indicating that the string pointed to by
.arg s1
is less than, equal to, or greater than the string pointed to by
.arg s2
.ct , according to the collating sequence selected.
:CMT. If an error occurs, these functions return
:CMT. .kw _NLSCMPERR
:CMT. .ct .li .
:CMT. The
:CMT. .kw _wcsnicoll
:CMT. function may set
:CMT. .kw errno
:CMT. to
:CMT. .kw EINVAL
:CMT. to indicate that either string contains characters outside of the
:CMT. collating sequence of the current code page.
.return end
.see begin
.seelist &function. _setmbcp strcoll stricmp _stricoll strncmp _strncoll strnicmp _strnicoll
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

char buffer[80] = "world";

void main()
  {
    int test;

    test = _strnicoll( buffer, "World2", 5 );
    if( test < 0 ) {
        printf( "Less than\n" );
    } else if( test == 0 ) {
        printf( "Equal\n" );
    } else {
        printf( "Greater than\n" );
    }
  }
.exmp end
.class WATCOM
.system
