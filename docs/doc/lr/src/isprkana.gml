.func isprkana
.synop begin
#include <jctype.h>
int isprkana( int c );
.ixfunc2 '&CharTest' &func
.synop end
.desc begin
The &func function tests if the argument
.arg c
is a single-byte printable character including space ("&sysrb.") or
single-byte Katakana character.
These are any characters for which the following expression is true:
.millust begin
isprint(c) || iskana(c)
.millust end
.desc end
.return begin
The &func function returns zero if the argument is not a single-byte printable
character (including space) or single-byte Katakana character;
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
    '.',
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
      printf( "Char is %sa single-byte "
              "printable character\n",
            ( isprkana( chars[i] ) ) ? "" : "not " );
    }
  }
.exmp output
Char is a single-byte printable character
Char is not a single-byte printable character
Char is a single-byte printable character
Char is a single-byte printable character
.exmp end
.class WATCOM
.system
