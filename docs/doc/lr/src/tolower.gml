.func tolower _tolower
.func2 towlower ISO C95
.func end
.synop begin
#include <ctype.h>
int tolower( int c );
.ixfunc2 '&CharTest' &funcb
.ixfunc2 '&Conversion' &funcb
int _tolower( int c );
.ixfunc2 '&CharTest' &_func
.ixfunc2 '&Conversion' &_func
.if &'length(&wfunc.) ne 0 .do begin
#include <wctype.h>
wint_t towlower( wint_t c );
.ixfunc2 '&CharTest' &wfunc
.ixfunc2 '&Conversion' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The
.id &funcb.
function converts
.arg c
to a lowercase letter if
.arg c
represents an uppercase letter.
.np
The
.id &_func.
function is a version of
.id &funcb.
to be used only when
.arg c
is known to be uppercase.
.im widefunc
.desc end
.return begin
The
.id &funcb.
function returns the corresponding lowercase letter when the
argument is an uppercase letter; otherwise, the original character is
returned.
.if &'length(&wfunc.) ne 0 .do begin
The
.id &wfunc.
function returns the corresponding wide-character lowercase
letter when the argument is a wide-character uppercase letter;
otherwise, the original wide character is returned.
.do end
.np
The result of
.id &_func.
is undefined if
.arg c
is not an uppercase letter.
.return end
.see begin
.im seeis
.seelist strlwr strupr tolower toupper
.see end
.exmp begin
#include <stdio.h>
#include <ctype.h>

char chars[] = {
    'A',
    '5',
    '$',
    'Z'
};
.exmp break
#define SIZE sizeof( chars ) / sizeof( char )
.exmp break
void main()
{
    int   i;
.exmp break
    for( i = 0; i < SIZE; i++ ) {
        printf( "%c ", tolower( chars[ i ] ) );
    }
    printf( "\n" );
}
.exmp output
a 5 $ z
.exmp end
.class ISO C
.system
