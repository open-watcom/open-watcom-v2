.func isprint iswprint _uisprint
.funcw iswprint
#include <ctype.h>
int isprint( int c );
.ixfunc2 '&CharTest' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <wctype.h>
int iswprint( wint_t c );
.ixfunc2 '&CharTest' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&ufunc.) ne 0 .do begin
int _uisprint( int c );
.ixfunc2 '&CharTest' &ufunc
.do end
.funcend
.desc begin
The &func function tests for any printable character including space
('&sysrb').
The
.kw isgraph
function is similar, except that the space character is excluded
from the character set being tested.
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is similar to &func except that it accepts a
wide-character argument.
.do end
.if &'length(&ufunc.) ne 0 .do begin
.np
The &ufunc Unicode function is identical to &func except that it
accepts a Unicode character argument.
.do end
.desc end
.return begin
The &func function returns a non-zero value when the argument is a
printable character.
.if &'length(&wfunc.) ne 0 .do begin
The &wfunc function returns a non-zero value when the argument is a
printable wide character.
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
        printf( "Char %c is %sa printable character\n",
                chars[i],
                ( isprint( chars[i] ) ) ? "" : "not " );
    }
}
.exmp output
Char A is a printable character
Char     is not a printable character
Char   is a printable character
Char } is a printable character
.exmp end
.class ANSI
.system
