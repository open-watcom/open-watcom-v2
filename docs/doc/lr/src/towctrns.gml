.func towctrans
.synop begin
#include <wctype.h>
wint_t towctrans( wint_t wc, wctrans_t desc );
.ixfunc2 '&Conversion' &func
.ixfunc2 '&Wide' &func
.synop end
.*
.desc begin
The
.id &func.
function maps the wide character
.arg wc
using the mapping described by
.arg desc
.ct .li .
Valid values of
.arg desc
are defined by the use of the
.kw wctrans
function.
.np
The two expressions listed below behave the same as a
call to the wide character case mapping function shown.
.begterm 20
.termhd1 Expression
.termhd2 Equivalent
.*
.termnx towctrans(wc, wctrans("tolower"))
towlower(wc)
.*
.termnx towctrans(wc, wctrans("toupper"))
towupper(wc)
.*
.endterm
.desc end
.*
.return begin
The
.id &func.
function returns the mapped value of
.arg wc
using the mapping described by
.arg desc
.ct .li .
.return end
.*
.see begin
.im seeis &function.
.see end
.*
.exmp begin
#include <stdio.h>
#include <wctype.h>

char *translations[2] = {
    "tolower",
    "toupper"
};
.exmp break
void main( void )
{
    int     i;
    wint_t  wc = 'A';
    wint_t  twc;
.exmp break
    for( i = 0; i < 2; i++ ) {
        twc = towctrans( wc, wctrans( translations[i] ) );
        printf( "%s(%lc): %lc\n", translations[i], wc, twc );
    }
}
.exmp output
tolower(A): a
toupper(A): A
.exmp end
.*
.class ANSI
.system
