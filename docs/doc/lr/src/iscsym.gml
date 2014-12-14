.func iscsym __iscsym __iswcsym
.synop begin
.funcw __iswcsym
#include <ctype.h>
int iscsym( int c );
.ixfunc2 '&CharTest' &func
int __iscsym( int c );
.ixfunc2 '&CharTest' &__func
.if &'length(&wfunc.) ne 0 .do begin
#include <wctype.h>
int __iswcsym( wint_t c );
.ixfunc2 '&CharTest' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The
.id &func.
function tests for a letter, underscore or digit.
.np
The
.id &__func.
function is identical to &func..
Use
.id &__func.
for ANSI/ISO naming conventions.
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
A non-zero value is returned when the character is a letter,
underscore or digit; otherwise, zero is returned.
.if &'length(&wfunc.) ne 0 .do begin
The
.id &wfunc.
function returns a non-zero value when
.arg c
is a wide-character representation of a letter, underscore or
digit character.
.do end
.return end
.see begin
.im seeis
.see end
.exmp begin
#include <stdio.h>
#include <ctype.h>

char chars[] = {
    'A',
    0x80,
    '_',
    '9',
    '+'
};
.exmp break
#define SIZE sizeof( chars ) / sizeof( char )
.exmp break
void main()
{
    int   i;
.exmp break
    for( i = 0; i < SIZE; i++ ) {
        printf( "Char %c is %sa C symbol character\n",
                chars[i],
                ( __iscsym( chars[i] ) ) ? "" : "not " );
    }
}
.exmp output
Char A is a C symbol character
Char   is not a C symbol character
Char _ is a C symbol character
Char 9 is a C symbol character
Char + is not a C symbol character
.exmp end
.class begin WATCOM
.ansiname &__func
.class end
.system
