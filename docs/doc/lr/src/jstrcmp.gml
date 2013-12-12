.func jstrcmp _fjstrcmp
.synop begin
#include <jstring.h>
int jstrcmp( const JCHAR *s1, const JCHAR *s2 );
.ixfunc2 '&Jstring' &func
.ixfunc2 '&Jcompare' &func
.if &farfnc eq 1 .do begin
int __far _fjstrcmp( const JCHAR __far *s1,
                     const JCHAR __far *s2 );
.ixfunc2 '&Jstring' &ffunc
.ixfunc2 '&Jcompare' &ffunc
.do end
.synop end
.desc begin
.if &farfnc eq 0 .do begin
The &func function compares
.do end
.el .do begin
The &func and &ffunc functions compare
.do end
the Kanji string pointed to by
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
.seelist jstrcmp jstricmp jstrncmp jstrnicmp
.seelist jstrcmp strcmp stricmp strncmp strnicmp
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
