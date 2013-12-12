.func isxdigit iswxdigit
.funcw iswxdigit
#include <ctype.h>
int isxdigit( int c );
.ixfunc2 '&CharTest' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
int iswxdigit( wint_t c );
.ixfunc2 '&CharTest' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The &func function tests for any hexadecimal-digit character.
These characters are the digits ('0' through '9') and the letters ('a'
through 'f') and ('A' through 'F').
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is similar to &func except that it accepts a
wide-character argument.
.do end
.desc end
.return begin
The &func function returns a non-zero value when the argument is a
hexadecimal-digit character.
.if &'length(&wfunc.) ne 0 .do begin
The &wfunc function returns a non-zero value when the argument is a
wide character that corresponds to a hexadecimal-digit character.
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
      printf( "Char %c is %sa hexadecimal digit"
            " character\n", chars[i],
            ( isxdigit( chars[i] ) ) ? "" : "not " );
    }
  }
.exmp output
Char A is a hexadecimal digit character
Char 5 is a hexadecimal digit character
Char $ is not a hexadecimal digit character
.exmp end
.class ANSI
.system
