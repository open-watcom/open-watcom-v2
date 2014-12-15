.func wctrans
.synop begin
#include <wctype.h>
wctrans_t wctrans( const char *property );
.ixfunc2 '&Conversion' &funcb
.ixfunc2 '&Wide' &funcb
.synop end
.*
.desc begin
The
.id &funcb.
function constructs a value with type
.kw wctrans_t
that describes a mapping between wide characters identified by the
string argument
.arg property
.ct .li .
The constructed value is affected by the
.kw LC_CTYPE
category of the current locale; the constructed value becomes
indeterminate if the category's setting is changed.
.np
The two strings listed below are valid in all locales as
.arg property
arguments to the
.id &funcb.
function.
.begterm 10
.termhd1 Constant
.termhd2 Meaning
.*
.term tolower
uppercase characters are mapped to lowercase
.*
.term toupper
lowercase characters are mapped to uppercase
.endterm
.desc end
.*
.return begin
If
.arg property
identifies a valid class of wide characters according to the
.kw LC_CTYPE
category of the current locale, the
.id &funcb.
function returns a non-zero
value that is valid as the second argument to the
.kw towctrans
function; otherwise, it returns zero.
.return end
.*
.see begin
.im seeis
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
