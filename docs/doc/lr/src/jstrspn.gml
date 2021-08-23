.func jstrspn _fjstrspn
.synop begin
#include <jstring.h>
size_t jstrspn( const JCHAR *s, const JCHAR *charset );
.ixfunc2 '&Jstring' &funcb
.ixfunc2 '&Jsearch' &funcb
.if &farfnc ne 0 .do begin
size_t _fjstrspn( const JCHAR __far *s, const JCHAR __far *charset );
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
.arg s
which consists of single-byte and double-byte characters from the Kanji
string pointed to by
.arg charset
.period
The terminating null character is not considered to be part of
.arg charset
.period
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
the length of the segment.
.return end
.see begin
.seelist jstrspn jstrcspn strspn strcspn
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>

void main()
  {
    printf( "%d\n", jstrspn( "out to lunch", "aeiou" ) );
    printf( "%d\n", jstrspn( "out to lunch", "xyz" ) );
  }
.exmp output
2
0
.exmp end
.class WATCOM
.system
