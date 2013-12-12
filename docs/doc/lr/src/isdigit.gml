.func isdigit iswdigit
.synop begin
.funcw iswdigit
#include <ctype.h>
int isdigit( int c );
.ixfunc2 '&CharTest' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <wctype.h>
int iswdigit( wint_t c );
.ixfunc2 '&CharTest' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The
.id &func.
function tests for any decimal-digit character '0' through
'9'.
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
function returns a non-zero value when the argument is a
decimal-digit character.
.if &'length(&wfunc.) ne 0 .do begin
The
.id &wfunc.
function returns a non-zero value when the argument is a
wide character corresponding to a decimal-digit character.
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
    '5',
    '$'
};
.exmp break
#define SIZE sizeof( chars ) / sizeof( char )
.exmp break
void main()
{
    int   i;
.exmp break
    for( i = 0; i < SIZE; i++ ) {
        printf( "Char %c is %sa digit character\n",
                chars[i],
                ( isdigit( chars[i] ) ) ? "" : "not " );
    }
}
.exmp output
Char A is not a digit character
Char 5 is a digit character
Char $ is not a digit character
.exmp end
.class ANSI
.system
