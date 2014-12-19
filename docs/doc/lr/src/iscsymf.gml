.func begin iscsymf __iscsymf
.func2 __iswcsymf
.func end
.synop begin
#include <ctype.h>
int iscsymf( int c );
.ixfunc2 '&CharTest' &funcb
int __iscsymf( int c );
.ixfunc2 '&CharTest' &funcb
.if &'length(&wfunc.) ne 0 .do begin
#include <wctype.h>
int __iswcsymf( wint_t c );
.ixfunc2 '&CharTest' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The
.id &funcb.
function tests for a letter or underscore.
.np
The
.id &__func.
function is identical to &funcb..
Use
.id &__func.
for ISO C naming conventions.
.if &'length(&wfunc.) ne 0 .do begin
.np
The
.id &wfunc.
function is similar to
.id &funcb.
except that it accepts a
wide-character argument.
.do end
.desc end
.return begin
A non-zero value is returned when the character is a letter or
underscore; otherwise, zero is returned.
.if &'length(&wfunc.) ne 0 .do begin
The
.id &wfunc.
function returns a non-zero value when
.arg c
is a wide-character representation of a letter or underscore character.
.do end
.return end
.see begin
.seelist isalpha isalnum iscntrl isdigit isgraph islower isprint
.seelist ispunct isspace isupper isxdigit tolower toupper
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
.ansiname &__func
.class WATCOM
.system
