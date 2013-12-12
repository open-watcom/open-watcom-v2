.func isalkana
.synop begin
#include <jctype.h>
int isalkana( int c );
.ixfunc2 '&CharTest' &func
.synop end
.desc begin
The
.id &func.
function tests if the argument
.arg c
is a single-byte alphabetic character ("a" to "z" or "A" to "Z") or
single-byte Katakana non-punctuation character.
These are any characters for which the following expression is true:
.millust begin
isalpha(c) || iskmoji(c)
.millust end
.desc end
.return begin
The
.id &func.
function returns zero if the argument is not a single-byte alphabetic
character or single-byte Katakana non-punctuation
character;
otherwise, a non-zero value is returned.
.return end
.see begin
.im seejis &function.
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>
#include <jctype.h>

JMOJI chars[] = {
    'A',
    0x9941,
    0xA4,
    0xA6
};
.exmp break
#define SIZE sizeof( chars ) / sizeof( JMOJI )
.exmp break
void main()
  {
    int   i;
.exmp break
    for( i = 0; i < SIZE; i++ ) {
      printf( "Char is %san alphabetic "
              "or Katakana character\n",
            ( isalkana( chars[i] ) ) ? "" : "not " );
    }
  }
.exmp output
Char is an alphabetic or Katakana character
Char is not an alphabetic or Katakana character
Char is not an alphabetic or Katakana character
Char is an alphabetic or Katakana character
.exmp end
.class WATCOM
.system
