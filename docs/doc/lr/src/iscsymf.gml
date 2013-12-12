.func iscsymf __iscsymf __iswcsymf
.funcw __iswcsymf
#include <ctype.h>
int iscsymf( int c );
.ixfunc2 '&CharTest' &func
int __iscsymf( int c );
.ixfunc2 '&CharTest' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <wctype.h>
int __iswcsymf( wint_t c );
.ixfunc2 '&CharTest' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The &func function tests for a letter or underscore.
.np
The &__func function is identical to &func..
Use &__func for ANSI/ISO naming conventions.
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is similar to &func except that it accepts a
wide-character argument.
.do end
.desc end
.return begin
A non-zero value is returned when the character is a letter or
underscore; otherwise, zero is returned.
.if &'length(&wfunc.) ne 0 .do begin
The &wfunc function returns a non-zero value when
.arg c
is a wide-character representation of a letter or underscore character.
.do end
.return end
.see begin
.seelist &function. isalpha isalnum iscntrl isdigit isgraph islower isprint
.seelist &function. ispunct isspace isupper isxdigit tolower toupper
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
        printf( "Char %c is %sa csymf character\n",
                chars[i],
                ( __iscsymf( chars[i] ) ) ? "" : "not " );
    }
}
.exmp output
Char A is a csymf character
Char   is not a csymf character
Char _ is a csymf character
Char 9 is not a csymf character
Char + is not a csymf character
.exmp end
.class begin WATCOM
.ansiname &__func
.class end
.system
