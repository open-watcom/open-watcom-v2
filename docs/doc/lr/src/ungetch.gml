.func ungetch
.synop begin
#include <conio.h>
int ungetch( int c );
.ixfunc2 '&KbIo' &funcb
.synop end
.desc begin
The
.id &funcb.
function pushes the character specified by
.arg c
back onto the input stream for the console.
This character will be returned by the next read from the console (with
.kw getch
or
.kw getche
functions) and will be detected by the function
.kw kbhit
.ct .li .
Only the last character returned in this way is remembered.
.pp
The
.id &funcb.
function clears the end-of-file indicator, unless the value of
.arg c
is
.kw EOF
.ct .li .
.desc end
.return begin
The
.id &funcb.
function returns the character pushed back.
.return end
.see begin
.im seeiocon
.see end
.exmp begin
#include <stdio.h>
#include <ctype.h>
#include <conio.h>

void main()
  {
    int c;
    long value;
.exmp break
    value = 0;
    c = getche();
    while( isdigit( c ) ) {
        value = value*10 + c - '0';
        c = getche();
    }
    ungetch( c );
    printf( "Value=%ld\n", value );
  }
.exmp end
.class WATCOM
.system
