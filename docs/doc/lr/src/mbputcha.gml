.func _mbputchar _fmbputchar
.synop begin
#include <mbstring.h>
unsigned char *_mbputchar( unsigned char *mbstr,
                           unsigned int dbch );
unsigned char far *_fmbputchar( unsigned char far *mbstr,
                                unsigned int dbch );
.synop end
.desc begin
The
.id &funcb.
function places the next single- or double-byte character
specified by
.arg dbch
at the start of the buffer specified by
.arg mbstr
.ct .li .
.im ffarfunc
.desc end
.return begin
The
.id &funcb.
function returns a pointer to the next location in which to
store a character.
.return end
.see begin
.seelist _mbsnccnt _mbgetcode _mbputchar
.see end
.exmp begin
#include <stdio.h>
#include <mbctype.h>
#include <mbstring.h>

void main()
  {
    unsigned int c;
    unsigned char *str1;
    unsigned char *str2;
    unsigned char buf[30];
.exmp break
    _setmbcp( 932 );
    str1 = "ab\x82\x62\x82\x63\ef\x81\x66";
    str2 = buf;

    for( ; *str1 != '\0'; ) {
        str1 = _mbgetcode( str1, &c );
        str2 = _mbputchar( str2, '<' );
        str2 = _mbputchar( str2, c );
        str2 = _mbputchar( str2, '>' );
    }
    *str2 = '\0';
    printf( "%s\n", buf );
  }
.exmp output
<a><b>< C>< D><e><f>< G>
.exmp end
.class WATCOM
.system
