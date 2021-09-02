.func _mbsnbicmp _fmbsnbicmp
.synop begin
#include <mbstring.h>
int _mbsnbicmp( const unsigned char *s1,
               const unsigned char *s2,
               size_t n );
.ixfunc2 '&String' &funcb
.ixfunc2 '&Compare' &funcb
.if &farfnc ne 0 .do begin
int _fmbsnbicmp( const unsigned char __far *s1,
                const unsigned char __far *s2,
                size_t n );
.ixfunc2 '&String' &ffunc
.ixfunc2 '&Compare' &ffunc
.do end
.synop end
.desc begin
The function lexicographically compares not more than
.arg n
bytes from the string pointed to by
.arg s1
to the string pointed to by
.arg s2
.period
The comparison is insensitive to case.
All uppercase characters from
.arg s1
and
.arg s2
are mapped to lowercase for the purposes of doing the comparison.
.farfuncp &ffunc. &funcb.
.desc end
.return begin
The function returns an integer less than, equal to, or greater
than zero, indicating that the string pointed to by
.arg s1
is less than, equal to, or greater than the string pointed to by
.arg s2
.period
.id &funcb.
is similar to
.reffunc _mbsnicmp
.ct , except that
.id &funcb.
compares strings by length in bytes rather than by length in characters.
.return end
.see begin
.seelist _mbsnbcat _mbsnbcmp strncmp _strnicmp
.see end
.exmp begin
#include <stdio.h>
#include <mbctype.h>
#include <mbstring.h>

const unsigned char str1[] = {
    0x81,0x40, /* double-byte space */
    0x82,0x60, /* double-byte A */
    0x82,0x79, /* double-byte Z */
    0x00
};

const unsigned char str2[] = {
    0x81,0x40, /* double-byte space */
    0x82,0x81, /* double-byte a */
    0x82,0x9a, /* double-byte z */
    0x00
};

void main()
  {
    _setmbcp( 932 );
    printf( "%d\n", _mbsnbicmp( str1, str2, 5 ) );
  }
.exmp output
0
.exmp end
.class WATCOM
.system
