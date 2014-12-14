.func nthctype _fnthctype
.synop begin
#include <jstring.h>
int nthctype( const char * str, size_t n );
.ixfunc2 '&Jstring' &func
.if &farfnc eq 1 .do begin
int far _fnthctype( const char far * str, size_t n );
.ixfunc2 '&Jstring' &ffunc
.do end
.synop end
.desc begin
.if &farfnc eq 0 .do begin
The
.id &func.
function returns
.do end
.el .do begin
The
.id &func.
and
.id &ffunc.
functions return
.do end
the type of an 8-bit character indicated as a byte offset
.arg n
from the start of the string
.arg str
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
.im ffarparm
.desc end
.return begin
If the character is a null character,
.id &func.
returns
.kw CT_ILGL
.ct .li .
If the offset
.arg n
is beyond the end of the string,
.id &func.
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
.return end
.see begin
.seelist chkctype iskanji iskanji2 jiszen nthctype
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>

char * types[4] = {
    "CT_ILGL",
    "CT_ANK",
    "CT_KJ1",
    "CT_KJ2"
};
.exmp break
void main()
  {
    int     i;
.exmp break
    for( i = 0; i < 9; i++ )
        printf( "%s\n",
            types[ 1 + nthctype( "abA¡Bc", i ) ]
            );
  }
.exmp output
CT_ANK
CT_ANK
CT_KJ1
CT_KJ2
CT_ANK
CT_KJ1
CT_KJ2
CT_ANK
CT_ILGL
.exmp end
.class WATCOM
.system
