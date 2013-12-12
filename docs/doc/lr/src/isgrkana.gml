.func isgrkana
.synop begin
#include <jctype.h>
int isgrkana( int c );
.ixfunc2 '&CharTest' &func
.synop end
.desc begin
The &func function tests if the argument
.arg c
is a single-byte printable character excluding space ("&sysrb.") or
single-byte Katakana character.
These are any characters for which the following expression is true:
.millust begin
isgraph(c) || iskana(c)
.millust end
.desc end
.return begin
The &func function returns zero if the argument is not a single-byte printable
character (excluding space) or single-byte Katakana character;
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
    ' '
};
.exmp break
#define SIZE sizeof( chars ) / sizeof( JMOJI )
.exmp break
void main()
  {
    int   i;
.exmp break
    for( i = 0; i < SIZE; i++ ) {
      printf( "Char is %sa single-byte printable "
              "non-space character\n",
            ( isgrkana( chars[i] ) ) ? "" : "not " );
    }
  }
.exmp output
Char is a single-byte printable non-space character
Char is not a single-byte printable non-space character
Char is a single-byte printable non-space character
Char is not a single-byte printable non-space character
.exmp end
.class WATCOM
.system
