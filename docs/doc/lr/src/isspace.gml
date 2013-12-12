.func isspace iswspace
.funcw iswspace
#include <ctype.h>
int isspace( int c );
.ixfunc2 '&CharTest' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <wctype.h>
int iswspace( wint_t c );
.ixfunc2 '&CharTest' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The &func function tests for the following white-space characters:
.begpoint $compact $setptnt 12
.termhd1 Constant
.termhd2 Character
.point ' '
space
.point '\f'
form feed
.point '\n'
new-line or linefeed
.point '\r'
carriage return
.point '\t'
horizontal tab
.point '\v'
vertical tab
.endpoint
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is similar to &func except that it accepts a
wide-character argument.
.do end
.desc end
.return begin
The &func function returns a non-zero character when the argument is
one of the indicated white-space characters.
.if &'length(&wfunc.) ne 0 .do begin
The &wfunc function returns a non-zero value when the argument is a
wide character that corresponds to a standard white-space character or
is one of an implementation-defined set of wide characters for which
.kw iswalnum
is false.
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
    0x09,
    ' ',
    0x7d
};
.exmp break
#define SIZE sizeof( chars ) / sizeof( char )
.exmp break
void main()
{
    int   i;
.exmp break
    for( i = 0; i < SIZE; i++ ) {
        printf( "Char %c is %sa space character\n",
                chars[i],
                ( isspace( chars[i] ) ) ? "" : "not " );
    }
}
.exmp output
Char A is not a space character
Char     is a space character
Char   is a space character
Char } is not a space character
.exmp end
.class ANSI
.system
