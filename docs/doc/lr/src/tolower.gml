.func tolower _tolower towlower
.synop begin
.funcw towlower
#include <ctype.h>
int tolower( int c );
.ixfunc2 '&CharTest' &func
.ixfunc2 '&Conversion' &func
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
The &func function converts
.arg c
to a lowercase letter if
.arg c
represents an uppercase letter.
.np
The &_func function is a version of &func to be used only when
.arg c
is known to be uppercase.
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is similar to &func except that it accepts a
wide-character argument.
.do end
.desc end
.return begin
The &func function returns the corresponding lowercase letter when the
argument is an uppercase letter; otherwise, the original character is
returned.
.if &'length(&wfunc.) ne 0 .do begin
The &wfunc function returns the corresponding wide-character lowercase
letter when the argument is a wide-character uppercase letter;
otherwise, the original wide character is returned.
.do end
.np
The result of &_func is undefined if
.arg c
is not an uppercase letter.
.return end
.see begin
.im seeis &function.
.seelist &function. strlwr strupr tolower toupper
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
.class ANSI
.system
