.func jstrncmp _fjstrncmp
.synop begin
#include <jstring.h>
int jstrncmp( const JCHAR *s1,
              const JCHAR *s2,
              size_t n );
.ixfunc2 '&Jstring' &funcb
.ixfunc2 '&Jcompare' &funcb
.if &farfnc eq 1 .do begin
int __far _fjstrncmp( const JCHAR __far *s1,
                      const JCHAR __far *s2,
                      size_t n );
.ixfunc2 '&Jstring' &ffunc
.ixfunc2 '&Jcompare' &ffunc
.do end
.synop end
.desc begin
.if &farfnc eq 0 .do begin
The
.id &funcb.
compares
.do end
.el .do begin
The
.id &funcb.
and
.id &ffunc.
functions compare
.do end
not more than
.arg n
single- or double-byte characters from the Kanji string pointed to by
.arg s1
to the Kanji string pointed to by
.arg s2
.ct .li .
The rule of comparison is:
.illust begin
ASCII and numeric characters < Katakana characters < Kanji characters
.illust end
.im ffarparm
.desc end
.return begin
.if &farfnc eq 0 .do begin
The
.id &funcb.
function returns
.do end
.el .do begin
The
.id &funcb.
and
.id &ffunc.
functions return
.do end
an integer less than, equal to, or greater than zero, indicating that
the Kanji string pointed to by
.arg s1
is less than, equal to, or greater than the Kanji string pointed to by
.arg s2
.ct .li .
.return end
.see begin
.seelist jstrncmp jstrcmp jstricmp jstrnicmp strcmp stricmp strncmp strnicmp
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>

void main()
  {
    printf( "%d\n", jstrncmp( "abcdef", "abcDEF", 10 ) );
    printf( "%d\n", jstrncmp( "abcdef", "abcDEF",  6 ) );
    printf( "%d\n", jstrncmp( "abcdef", "abcDEF",  3 ) );
    printf( "%d\n", jstrncmp( "abcdef", "abcDEF",  0 ) );
  }
.exmp output
1
1
0
0
.exmp end
.class WATCOM
.system
