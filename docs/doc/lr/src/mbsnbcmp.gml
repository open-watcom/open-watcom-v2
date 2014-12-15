.func _mbsnbcmp _fmbsnbcmp
.synop begin
#include <mbstring.h>
int _mbsnbcmp( const unsigned char *s1,
               const unsigned char *s2,
               size_t n );
.ixfunc2 '&String' &funcb
.ixfunc2 '&Compare' &funcb
.if &farfnc eq 1 .do begin
int _fmbsnbcmp( const unsigned char __far *s1,
                const unsigned char __far *s2,
                size_t n );
.ixfunc2 '&String' &ffunc
.ixfunc2 '&Compare' &ffunc
.do end
.synop end
.desc begin
The
.id &funcb.
lexicographically compares not more than
.arg n
bytes from the string pointed to by
.arg s1
to the string pointed to by
.arg s2
.ct .li .
.im farparm
.desc end
.return begin
The
.id &funcb.
function returns an integer less than, equal to, or greater
than zero, indicating that the string pointed to by
.arg s1
is less than, equal to, or greater than the string pointed to by
.arg s2
.ct .li .
.id &funcb.
is similar to
.kw _mbsncmp
.ct , except that
.id &funcb.
compares strings by bytes rather than by characters.
.return end
.see begin
.seelist _mbsnbcat _mbsnbcmp _mbsnbicmp strncmp strnicmp
.see end
.exmp begin
#include <stdio.h>
#include <mbctype.h>
#include <mbstring.h>

const unsigned char str1[] = {
    0x81,0x40, /* double-byte space */
    0x82,0x60, /* double-byte A */
    0x00
};

const unsigned char str2[] = {
    0x81,0x40, /* double-byte space */
    0x82,0xA6, /* double-byte Hiragana */
    0x83,0x42, /* double-byte Katakana */
    0x00
};

void main()
  {
    _setmbcp( 932 );
    printf( "%d\n", _mbsnbcmp( str1, str2, 3 ) );
  }
.exmp output
0
.exmp end
.class WATCOM
.system
