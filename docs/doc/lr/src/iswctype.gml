.func iswctype
#include <wctype.h>
int iswctype( wint_t wc, wctype_t desc );
.ixfunc2 '&CharTest' &wfunc
.ixfunc2 '&Wide' &wfunc
.synop end
.*
.desc begin
The &func function determines whether the wide character
.arg wc
has the property described by
.arg desc
.ct .li .
Valid values of
.arg desc
are defined by the use of the
.kw wctype
function.
.np
The twelve expressions listed below have a truth-value equivalent to a
call to the wide character testing function shown.
.begterm 20
.termhd1 Expression
.termhd2 Equivalent
.*
.termnx iswctype(wc, wctype("alnum"))
iswalnum(wc)
.*
.termnx iswctype(wc, wctype("alpha"))
iswalpha(wc)
.*
.termnx iswctype(wc, wctype("blank"))
iswblank(wc)
.*
.termnx iswctype(wc, wctype("cntrl"))
iswcntrl(wc)
.*
.termnx iswctype(wc, wctype("digit"))
iswdigit(wc)
.*
.termnx iswctype(wc, wctype("graph"))
iswgraph(wc)
.*
.termnx iswctype(wc, wctype("lower"))
iswlower(wc)
.*
.termnx iswctype(wc, wctype("print"))
iswprint(wc)
.*
.termnx iswctype(wc, wctype("punct"))
iswpunct(wc)
.*
.termnx iswctype(wc, wctype("space"))
iswspace(wc)
.*
.termnx iswctype(wc, wctype("upper"))
iswupper(wc)
.*
.termnx iswctype(wc, wctype("xdigit"))
iswxdigit(wc)
.endterm
.desc end
.*
.return begin
The &func function returns non-zero (true) if and only if the value of
the wide character
.arg wc
has the property described by
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

char *types[] = {
    "alnum",
    "alpha",
    "blank",
    "cntrl",
    "digit",
    "graph",
    "lower",
    "print",
    "punct",
    "space",
    "upper",
    "xdigit"
};
.exmp break
void main( void )
{
    int     i;
    wint_t  wc = 'A';
.exmp break
    for( i = 0; i < 12; i++ )
        if( iswctype( wc, wctype( types[i] ) ) )
            printf( "%s\n", types[i] );
}
.exmp output
alnum
alpha
graph
print
upper
xdigit
.exmp end
.*
.class ANSI
.system
