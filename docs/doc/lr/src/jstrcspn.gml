.func jstrcspn _fjstrcspn
.synop begin
#include <string.h>
size_t jstrcspn( const JCHAR *str,
                 const JCHAR *charset );
.ixfunc2 '&Jstring' &funcb
.ixfunc2 '&Jsearch' &funcb
.if &farfnc eq 1 .do begin
size_t __far _fjstrcspn( const JCHAR __far *str,
                         const JCHAR __far *charset );
.ixfunc2 '&Jstring' &ffunc
.ixfunc2 '&Jsearch' &ffunc
.do end
.synop end
.desc begin
.if &farfnc eq 0 .do begin
The
.id &funcb.
function computes
.do end
.el .do begin
The
.id &funcb.
and
.id &ffunc.
functions compute
.do end
the length of the initial segment of the Kanji string pointed to by
.arg str
which consists entirely of characters
.us not
from the Kanji string pointed to by
.arg charset
.ct .li .
A double-byte character is treated as one character.
The terminating null character is not considered part of
.arg str
.ct .li .
.im ffarparm
.desc end
.return begin
The length of the initial segment is returned.
.return end
.see begin
.seelist jstrcspn jstrspn strcspn strspn
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>

void main()
  {
    printf( "%d\n", jstrcspn( "abcbcadef", "cba" ) );
    printf( "%d\n", jstrcspn( "xxxbcadef", "cba" ) );
    printf( "%d\n", jstrcspn( "123456789", "cba" ) );
  }
.exmp output
0
3
9
.exmp end
.class WATCOM
.system
