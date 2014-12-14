.func isblank iswblank
.synop begin
.funcw iswblank
#include <ctype.h>
int isblank( int c );
.ixfunc2 '&CharTest' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <wctype.h>
int iswblank( wint_t c );
.ixfunc2 '&CharTest' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The
.id &func.
function tests for the following blank characters:
.begpoint $compact $setptnt 12
.termhd1 Constant
.termhd2 Character
.point ' '
space
.point '\t'
horizontal tab
.endpoint
.if &'length(&wfunc.) ne 0 .do begin
.np
The
.id &wfunc.
function is similar to
.id &func.
except that it accepts a
wide-character argument.
.do end
.desc end
.return begin
The
.id &func.
function returns a non-zero character when the argument is
one of the indicated blank characters.
.if &'length(&wfunc.) ne 0 .do begin
The
.id &wfunc.
function returns a non-zero value when the argument is a
wide character that corresponds to a standard blank character or
is one of an implementation-defined set of wide characters for which
.kw iswalnum
is false.
.do end
Otherwise, zero is returned.
.return end
.see begin
.im seeis
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
      printf( "Char %c is %sa blank character\n",
            chars[i],
            ( isblank( chars[i] ) ) ? "" : "not " );
    }
}
.exmp output
Char A is not a blank character
Char     is a blank character
Char   is a blank character
Char } is not a blank character
.exmp end
.class ANSI
.system
