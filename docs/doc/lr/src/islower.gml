.func islower iswlower
.funcw iswlower
#include <ctype.h>
int islower( int c );
.ixfunc2 '&CharTest' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <wctype.h>
int iswlower( wint_t c );
.ixfunc2 '&CharTest' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.funcend
.desc begin
The &func function tests for any lowercase letter 'a' through 'z'.
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is similar to &func except that it accepts a
wide-character argument.
.do end
.desc end
.return begin
The &func function returns a non-zero value when argument is a lowercase
letter.
.if &'length(&wfunc.) ne 0 .do begin
The &wfunc function returns a non-zero value when the argument is a
wide character that corresponds to a lowercase letter, or if it is one
of an implementation-defined set of wide characters for which none of
.kw iswcntrl
.ct ,
.kw iswdigit
.ct ,
.kw iswpunct
.ct , or
.kw iswspace
is true.
.do end
Otherwise, zero is returned.
.return end
.see begin
.im seeis &function.
.see end
.exmp begin
#include <stdio.h>
#include <ctype.h>

char chars[] = {
    'A',
    'a',
    'z',
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
        printf( "Char %c is %sa lowercase character\n",
                chars[i],
                ( islower( chars[i] ) ) ? "" : "not " );
    }
}
.exmp output
Char A is not a lowercase character
Char a is a lowercase character
Char z is a lowercase character
Char Z is not a lowercase character
.exmp end
.class ANSI
.system
