.func isalnum iswalnum
.funcw iswalnum
#include <ctype.h>
int isalnum( int c );
.ixfunc2 '&CharTest' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <wctype.h>
int iswalnum( wint_t c );
.ixfunc2 '&CharTest' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The &func function tests if the argument
.arg c
is an alphanumeric character ('a' to 'z', 'A' to 'Z', or '0' to '9').
An alphanumeric character is any character for which
.kw isalpha
or
.kw isdigit
is true.
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is similar to &func except that it accepts a
wide-character argument.
.do end
.desc end
.return begin
The &func function returns zero if the argument is neither an alphabetic
character (A-Z or a-z) nor a digit (0-9).
Otherwise, a non-zero value is returned.
.if &'length(&wfunc.) ne 0 .do begin
The &wfunc function returns a non-zero value if either
.kw iswalpha
or
.kw iswdigit
is true for
.arg c
.ct .li .
.do end
.return end
.see begin
.im seeis &function.
.see end
.exmp begin
#include <stdio.h>
#include <ctype.h>

void main()
{
    if( isalnum( getchar() ) ) {
        printf( "is alpha-numeric\n" );
    }
}
.exmp end
.class ANSI
.system
