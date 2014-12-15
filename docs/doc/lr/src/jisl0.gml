.func jisl0
.synop begin
#include <jstring.h>
int jisl0( JMOJI c );
.ixfunc2 '&CharTest' &funcb
.synop end
.desc begin
The
.id &funcb.
function tests if the argument
.arg c
is in the set of double-byte characters that include Hiragana,
Katakana, punctuation symbols, graphical symbols, Roman and Cyrillic
alphabets, etc.
Double-byte Kanji characters are not in this set.
These are any characters for which the following expression is true:
.millust begin
0x8140 <= c <= 0x889E
.millust end
.desc end
.return begin
The
.id &funcb.
function returns zero if the argument is not in the set;
otherwise, a non-zero value is returned.
.return end
.see begin
.im seejto
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>

JMOJI chars[] = {
    0x8143,
    0x8183,
    0x8254,
    0x8277
};
.exmp break
#define SIZE sizeof( chars ) / sizeof( JMOJI )
.exmp break
void main()
  {
    int   i;
    JMOJI c;
.exmp break
    for( i = 0; i < SIZE; i++ ) {
        c = chars[ i ];
        printf( "%c%c", c>>8, c );
        if( jisl0( c ) ) {
          printf( " is Level 0 JIS" );
        }
        printf( "\n" );
    }
  }
.exmp output
,  is Level 0 JIS
<  is Level 0 JIS
5  is Level 0 JIS
X  is Level 0 JIS
.exmp end
.class WATCOM
.system
