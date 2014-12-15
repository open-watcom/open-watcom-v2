.func chkctype
.synop begin
#include <jstring.h>
int chkctype( JCHAR c, int mode );
.ixfunc2 '&Jstring' &funcb
.synop end
.desc begin
The
.id &funcb.
function identifies the type of an 8-bit character.
.np
If
.arg mode
is not
.kw CT_KJ1
then
.id &funcb.
identifies whether
.arg c
is the first byte of a double-byte Kanji character.
If
.arg c
is the first byte of a double-byte Kanji character, it returns
.kw CT_KJ1
.ct .li .
If
.arg c
is not the first byte of a double-byte Kanji character, it returns
.kw CT_ANK
or
.kw CT_ILGL
.ct .li .
.np
If
.arg mode
is
.kw CT_KJ1
then
.id &funcb.
identifies whether
.arg c
is the second byte of a double-byte Kanji character.
If
.arg c
is the second byte of a double-byte Kanji character, it returns
.kw CT_KJ2
.ct .li .
If
.arg c
is not the second byte of a double-byte Kanji character, it returns
.kw CT_ILGL
.ct .li .
Note that the
.kw nthctype
function identifies the type of the n'th byte of a Kanji string.
.desc end
.return begin
If
.arg c
is a null character,
.id &funcb.
returns
.kw CT_ILGL
.ct .li .
In all other cases, it returns one of
.kw CT_ANK
.ct ,
.kw CT_KJ1
.ct ,
.kw CT_KJ2
.ct , or
.kw CT_ILGL
.ct .li .
.begterm 10
.termhd1 Constant
.termhd2 Meaning
.term CT_ANK
(0) The character is a single-byte character (i.e., the byte is not
valid as first byte of a double-byte character).
.term CT_KJ1
(1) The byte is valid as the first byte of a double-byte character.
.term CT_KJ2
(2) The byte is valid as the second byte of a double-byte character
(this value may be returned only when
.arg mode
is
.kw CT_KJ1
.ct ).
.term CT_ILGL
(-1) The byte is not valid as second byte of a double-byte character
or it is the null character,
.endterm
.return end
.see begin
.seelist chkctype nthctype
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>

static void checkKanji( JMOJI c )
  {
    if( chkctype( c >> 8, CT_ANK ) == CT_KJ1 ) {
      if( chkctype( c & 0xFF, CT_KJ1 ) == CT_KJ2 ) {
        printf( "Char is double-byte Kanji\n" );
      } else {
        printf( "Char is illegal\n" );
      }
    } else {
        printf( "Char is not double-byte Kanji\n" );
    }
  }
.exmp break
void main()
  {
    checkKanji( 'a' );
    checkKanji( (0x81<<8) + 0x55 );
    checkKanji( (0x81<<8) + 0x10 );
  }
.exmp output
Char is not double-byte Kanji
Char is double-byte Kanji
Char is illegal
.exmp end
.class WATCOM
.system
