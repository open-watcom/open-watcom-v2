.func jstrnicmp _fjstrnicmp
.synop begin
#include <jstring.h>
int jstrnicmp( const JCHAR *s1, const JCHAR *s2, size_t len );
.ixfunc2 '&Jstring' &funcb
.ixfunc2 '&Jcompare' &funcb
.if &farfnc ne 0 .do begin
int _fjstrnicmp( const JCHAR __far *s1, const JCHAR __far *s2, size_t len );
.ixfunc2 '&Jstring' &ffunc
.ixfunc2 '&Jcompare' &ffunc
.do end
.synop end
.desc begin
The function compares, without case sensitivity,
the Kanji string pointed to by
.arg s1
to the Kanji string pointed to by
.arg s2
.ct , for at most
.arg len
characters.
All ASCII characters are less than any 1-byte Katakana chracters.
All 1-byte Katakana charaters are less than any 2-byte Kanji
characters.
All uppercase Roman characters (A-Z) from
.arg s1
and
.arg s2
are mapped to lowercase for the purposes of doing the comparison.
.farfuncp &ffunc. &funcb.
.desc end
.return begin
The function returns an integer less than, equal to,
or greater than zero, indicating that the Kanji string pointed to by
.arg s1
is less than, equal to, or greater than the Kanji string pointed to by
.arg s2
.period
.return end
.see begin
.seelist jstrnicmp jstrcmp jstricmp jstrncmp _strnicmp strcmp _stricmp strncmp
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>

void main()
  {
    printf( "%d\n", jstrnicmp( "abcdef", "ABCXXX", 10 ) );
    printf( "%d\n", jstrnicmp( "abcdef", "ABCXXX",  6 ) );
    printf( "%d\n", jstrnicmp( "abcdef", "ABCXXX",  3 ) );
    printf( "%d\n", jstrnicmp( "abcdef", "ABCXXX",  0 ) );
  }
.exmp output
-20
-20
0
0
.exmp end
.class WATCOM
.system
