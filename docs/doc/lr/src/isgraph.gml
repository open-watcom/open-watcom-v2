.func begin isgraph
.func2 iswgraph ISO C95
.func end
.synop begin
#include <ctype.h>
int isgraph( int c );
.ixfunc2 '&CharTest' &funcb
.if &'length(&wfunc.) ne 0 .do begin
#include <wctype.h>
int iswgraph( wint_t c );
.ixfunc2 '&CharTest' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The
.id &funcb.
function tests for any printable character except space
('&sysrb').
The
.kw isprint
function is similar, except that the space character is also included
in the character set being tested.
.im widefun2
.desc end
.return begin
The
.id &funcb.
function returns non-zero when the argument is a printable
character (except a space).
.if &'length(&wfunc.) ne 0 .do begin
The
.id &wfunc.
function returns a non-zero value when the argument is a
printable wide character (except a wide-character space).
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
        printf( "Char %c is %sa printable character\n",
                chars[i],
                ( isgraph( chars[i] ) ) ? "" : "not " );
    }
}
.exmp output
Char A is a printable character
Char     is not a printable character
Char   is not a printable character
Char } is a printable character
.exmp end
.class ISO C
.system
