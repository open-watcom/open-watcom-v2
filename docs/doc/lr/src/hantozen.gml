.func hantozen
#include <jstring.h>
JMOJI hantozen( JMOJI c );
.ixfunc2 '&Jstring' &func
.funcend
.desc begin
The &func function returns the double-byte character equivalent to the
single-byte ASCII character
.arg c
.ct .li .
The ASCII character must be in the range 0x20 to 0x7E.
.desc end
.return begin
The &func function returns
.arg c
if there is no equivalent double-byte character;
otherwise &func returns a double-byte character.
.return end
.see begin
.im seejis &function.
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>

char alphabet[] = {
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
};
.exmp break
void main()
  {
    int   i;
    JMOJI c;
.exmp break
    for( i = 0; i < sizeof( alphabet ) - 1; i++ ) {
        c = hantozen( alphabet[ i ] );
        printf( "%c%c", c>>8, c );
    }
    printf( "\n" );
  }
.exmp output
A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
.exmp end
.class WATCOM
.system
