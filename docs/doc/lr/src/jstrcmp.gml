.func jstrcmp _fjstrcmp
.synop begin
#include <jstring.h>
int jstrcmp( const JCHAR *s1, const JCHAR *s2 );
.ixfunc2 '&Jstring' &funcb
.ixfunc2 '&Jcompare' &funcb
.if &farfnc ne 0 .do begin
int _fjstrcmp( const JCHAR __far *s1, const JCHAR __far *s2 );
.ixfunc2 '&Jstring' &ffunc
.ixfunc2 '&Jcompare' &ffunc
.do end
.synop end
.desc begin
.if &farfnc eq 0 .do begin
The
.id &funcb.
function compares
.do end
.el .do begin
The
.id &funcb.
and
.id &ffunc.
functions compare
.do end
the Kanji string pointed to by
.arg s1
to the Kanji string pointed to by
.arg s2
.period
The rule of comparison is:
.illust begin
ASCII and numeric characters < Katakana characters < Kanji characters
.illust end
.farfuncp &ffunc. &funcb.
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
.period
.return end
.see begin
.seelist jstrcmp jstricmp jstrncmp jstrnicmp
.seelist strcmp _stricmp strncmp _strnicmp
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>

void main()
  {
    printf( "%d\n", jstrcmp( "abcdef", "abcdef" ) );
    printf( "%d\n", jstrcmp( "abcdef", "abc" ) );
    printf( "%d\n", jstrcmp( "abc", "abcdef" ) );
    printf( "%d\n", jstrcmp( "abcdef", "mnopqr" ) );
    printf( "%d\n", jstrcmp( "mnopqr", "abcdef" ) );
  }
.exmp output
0
1
-1
-1
1
.exmp end
.class WATCOM
.system
