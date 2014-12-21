.func begin isxdigit
.func2 iswxdigit ISO C95
.func end
.synop begin
#include <ctype.h>
int isxdigit( int c );
.ixfunc2 '&CharTest' &funcb
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
int iswxdigit( wint_t c );
.ixfunc2 '&CharTest' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The
.id &funcb.
function tests for any hexadecimal-digit character.
These characters are the digits ('0' through '9') and the letters ('a'
through 'f') and ('A' through 'F').
.im widefun2
.desc end
.return begin
The
.id &funcb.
function returns a non-zero value when the argument is a
hexadecimal-digit character.
.if &'length(&wfunc.) ne 0 .do begin
The
.id &wfunc.
function returns a non-zero value when the argument is a
wide character that corresponds to a hexadecimal-digit character.
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
.class ISO C
.system
