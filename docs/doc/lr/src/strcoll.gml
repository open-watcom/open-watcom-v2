.func strcoll wcscoll _mbscoll
.synop begin
#include <string.h>
int strcoll( const char *s1, const char *s2 );
.ixfunc2 '&String' &funcb
.ixfunc2 '&Compare' &funcb
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
int wcscoll( const wchar_t *s1, const wchar_t *s2 );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Compare' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&mfunc.) ne 0 .do begin
#include <mbstring.h>
int _mbscoll( const unsigned char *s1, const unsigned char *s2 );
.ixfunc2 '&String' &mfunc
.ixfunc2 '&Compare' &mfunc
.ixfunc2 '&Multibyte' &mfunc
.do end
.synop end
.desc begin
The
.id &funcb.
function compares the string pointed to by
.arg s1
to the string pointed to by
.arg s2
.ct .li .
The comparison uses the collating sequence selected by the
.kw setlocale
function.
The function will be equivalent to the
.kw strcmp
function when the collating sequence is selected from the
.mono "C"
locale.
.im widefun1
.im mbsfunc
.desc end
.return begin
The
.id &funcb.
function returns an integer less than, equal to, or
greater than zero, indicating that the string pointed to by
.arg s1
is less than, equal to, or greater than the string pointed to by
.arg s2
.ct , according to the collating sequence selected.
.return end
.see begin
.seelist strcoll setlocale strcmp strncmp
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

char buffer[80] = "world";

void main()
  {
    if( strcoll( buffer, "Hello" ) < 0 ) {
        printf( "Less than\n" );
    }
  }
.exmp end
.class ISO C
.system
