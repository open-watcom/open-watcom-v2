.func _mbgetcode _fmbgetcode
.synop begin
#include <mbstring.h>
unsigned char *_mbgetcode( unsigned char *mbstr,
                           unsigned int *dbchp );
.if &farfnc eq 1 .do begin
unsigned char far *_fmbgetcode( unsigned char far *mbstr,
                                unsigned int *dbchp );
.do end
.synop end
.desc begin
The
.id &func.
function places the next single- or double-byte character
from the start of the Kanji string specified by
.arg mbstr
in the wide character pointed to by
.arg dbchp
.ct .li .
If the second-half of a double-byte character is NULL, then the
returned wide character is NULL.
.im ffarfunc
.desc end
.return begin
The
.id &func.
function returns a pointer to the next character to be
obtained from the string.
If
.arg mbstr
points at a null character then
.arg mbstr
is returned.
.return end
.see begin
.seelist &function. _mbsnccnt _mbgetcode _mbputchar
.see end
.exmp begin
#include <stdio.h>
#include <mbctype.h>
#include <mbstring.h>

unsigned char set[] = {
    "ab\x81\x41\x81\x42\cd\x81"
};
.exmp break
void main()
  {
    unsigned int c;
    unsigned char *str;
.exmp break
    _setmbcp( 932 );
    str = set;
    for( ; *str != '\0'; ) {
        str = _mbgetcode( str, &c );
        printf( "Character code 0x%2.2x\n", c );
    }
  }
.exmp output
Character code 0x61
Character code 0x62
Character code 0x8141
Character code 0x8142
Character code 0x63
Character code 0x64
Character code 0x00
.exmp end
.class WATCOM
.system
