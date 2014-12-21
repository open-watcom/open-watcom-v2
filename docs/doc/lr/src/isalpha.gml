.func begin isalpha
.func2 iswalpha ISO C95
.func end
.synop begin
#include <ctype.h>
int isalpha( int c );
.ixfunc2 '&CharTest' &funcb
.if &'length(&wfunc.) ne 0 .do begin
#include <wctype.h>
int iswalpha( wint_t c );
.ixfunc2 '&CharTest' &wfunc
.do end
.synop end
.desc begin
The
.id &funcb.
function tests if the argument
.arg c
is an alphabetic character ('a' to 'z' and 'A' to 'Z').
An alphabetic character is any character for which
.kw isupper
or
.kw islower
is true.
.im widefun2
.desc end
.return begin
The
.id &funcb.
function returns zero if the argument is not an alphabetic
character (A-Z or a-z);
otherwise, a non-zero value is returned.
.if &'length(&wfunc.) ne 0 .do begin
The
.id &wfunc.
function returns a non-zero value only for wide characters
for which
.kw iswupper
or
.kw iswlower
is true, or any wide character that is one of an
implementation-defined set for which none of
.kw iswcntrl
.ct ,
.kw iswdigit
.ct ,
.kw iswpunct
.ct , or
.kw iswspace
is true.
.do end
.return end
.see begin
.im seeis
.see end
.exmp begin
#include <stdio.h>
#include <ctype.h>

void main()
{
    if( isalpha( getchar() ) ) {
        printf( "is alphabetic\n" );
    }
}
.exmp end
.class ISO C
.system
