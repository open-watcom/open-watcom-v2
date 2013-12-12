.func jstricmp _fjstricmp
#include <jstring.h>
int jstricmp( const JCHAR *s1, const JCHAR *s2 );
.ixfunc2 '&Jstring' &func
.ixfunc2 '&Jcompare' &func
.if &farfnc eq 1 .do begin
int __far _fjstricmp( const JCHAR __far *s1,
                      const JCHAR __far *s2 );
.ixfunc2 '&Jstring' &ffunc
.ixfunc2 '&Jcompare' &ffunc
.do end
.synop end
.desc begin
.if &farfnc eq 0 .do begin
The &func function compares,
.do end
.el .do begin
The &func and &ffunc functions compare,
.do end
with case insensitivity, the Kanji string pointed to by
.arg s1
to the Kanji string pointed to by
.arg s2
.ct .li .
All uppercase Roman characters (A-Z) from
.arg s1
and
.arg s2
are mapped to lowercase for the purposes of doing the comparison.
The rule of comparison is:
.illust begin
ASCII and numeric characters < Katakana characters < Kanji characters
.illust end
.im ffarparm
.desc end
.return begin
.if &farfnc eq 0 .do begin
The &func function returns
.do end
.el .do begin
The &func and &ffunc functions return
.do end
an integer less than, equal to, or greater than zero, indicating that
the Kanji string pointed to by
.arg s1
is less than, equal to, or greater than the Kanji string pointed to by
.arg s2
.ct .li .
.return end
.see begin
.seelist jstricmp strcmp strcmpi strncmp strnicmp
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>

void main()
  {
    printf( "%d\n", jstricmp( "AbCDEF", "abcdef" ) );
    printf( "%d\n", jstricmp( "abcdef", "ABC"    ) );
    printf( "%d\n", jstricmp( "abc",    "ABCdef" ) );
    printf( "%d\n", jstricmp( "Abcdef", "mnopqr" ) );
    printf( "%d\n", jstricmp( "Mnopqr", "abcdef" ) );
  }
.exmp output
0
100
-100
-12
12
.exmp end
.class WATCOM
.system
