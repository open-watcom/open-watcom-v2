.func zentohan
.synop begin
#include <jstring.h>
JMOJI zentohan( JMOJI c );
.ixfunc2 '&Jstring' &func
.synop end
.desc begin
The
.id &func.
function returns the single-byte ASCII character equivalent
to the double-byte character
.arg c
.ct .li .
The ASCII character will be in the range 0x20 to 0x7E.
.desc end
.return begin
The
.id &func.
function returns
.arg c
if there is no equivalent single-byte ASCII character;
otherwise
.id &func.
returns a single-byte ASCII character.
.return end
.see begin
.im seejis &function.
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>

#define ZEN(x) 130*256+(x-1)
.exmp break
JMOJI alphabet[26] = {
    /*   A        B        C        D        E */
    ZEN('a'),ZEN('b'),ZEN('c'),ZEN('d'),ZEN('e'),
    /*   F        G        H        I        J */
    ZEN('f'),ZEN('g'),ZEN('h'),ZEN('i'),ZEN('j'),
    /*   K        L        M        N        O */
    ZEN('k'),ZEN('l'),ZEN('m'),ZEN('n'),ZEN('o'),
    /*   P        Q        R        S        T */
    ZEN('p'),ZEN('q'),ZEN('r'),ZEN('s'),ZEN('t'),
    /*   U        V        W        X        Y */
    ZEN('u'),ZEN('v'),ZEN('w'),ZEN('x'),ZEN('y'),
    /*   Z                                    */
    ZEN('z')
};
.exmp break
void main()
  {
    int   i;
    char  c;
.exmp break
    for( i = 0;
         i < sizeof( alphabet )/sizeof(JMOJI);
         i++ ) {
        c = zentohan( alphabet[ i ] );
        printf( "%c", c );
    }
    printf( "\n" );
  }
.exmp output
ABCDEFGHIJKLMNOPQRSTUVWXYZ
.exmp end
.class WATCOM
.system
