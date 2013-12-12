.func iscntrl iswcntrl
.synop begin
.funcw iswcntrl
#include <ctype.h>
int iscntrl( int c );
.ixfunc2 '&CharTest' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
int iswcntrl( wint_t c );
.ixfunc2 '&CharTest' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The
.id &func.
function tests for any control character.
A control character is any character whose value is from 0 through 31.
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
control character.
.if &'length(&wfunc.) ne 0 .do begin
The
.id &wfunc.
function returns a non-zero value when the argument is a
control wide character.
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
        printf( "Char %c is %sa Control character\n",
                chars[i],
                ( iscntrl( chars[i] ) ) ? "" : "not " );
    }
}
.exmp output
Char A is not a Control character
Char     is a Control character
Char Z is not a Control character
.exmp end
.class ANSI
.system
