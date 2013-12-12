.func isascii __isascii iswascii
.funcw iswascii
#include <ctype.h>
int isascii( int c );
.ixfunc2 '&CharTest' &func
int __isascii( int c );
.ixfunc2 '&CharTest' &__func
.if &'length(&wfunc.) ne 0 .do begin
#include <wctype.h>
int iswascii( wint_t c );
.ixfunc2 '&CharTest' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The &func function tests for a character in the range from 0 to 127.
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
The &func function returns a non-zero value when the character is in
the range 0 to 127; otherwise, zero is returned.
.if &'length(&wfunc.) ne 0 .do begin
The &wfunc function returns a non-zero value when
.arg c
is a wide-character representation of an ASCII character.
.do end
.return end
.see begin
.im seeis &function.
.see end
.exmp begin
#include <stdio.h>
#include <ctype.h>

char chars[] = {
    'A',
    0x80,
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
        printf( "Char %c is %san ASCII character\n",
                chars[i],
                ( isascii( chars[i] ) ) ? "" : "not " );
    }
}
.exmp output
Char A is an ASCII character
Char   is not an ASCII character
Char Z is an ASCII character
.exmp end
.class begin WATCOM
.ansiname &__func
.class end
.system
