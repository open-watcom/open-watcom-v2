.func jstrmatch _fjstrmatch
.synop begin
#include <jstring.h>
JSTRING jstrmatch( const JCHAR *s, const JCHAR *charset );
.ixfunc2 '&Jstring' &funcb
.ixfunc2 '&Jsearch' &funcb
.if &farfnc ne 0 .do begin
FJSTRING _fjstrmatch( const JCHAR __far *s, const JCHAR __far *charset );
.ixfunc2 '&Jstring' &ffunc
.ixfunc2 '&Jsearch' &ffunc
.do end
.synop end
.desc begin
.if &farfnc eq 0 .do begin
The
.id &funcb.
function locates
.do end
.el .do begin
The
.id &funcb.
and
.id &ffunc.
functions locate
.do end
the first occurrence in the Kanji string pointed to by
.arg s
of a single-byte or double-byte character that appears
in the Kanji string pointed to by
.arg charset
.period
.farfunc &ffunc. &funcb.
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
a pointer to the located character, or
.mono NULL
if no character is found.
.return end
.see begin
.seelist jstrmatch jstrskip jstrspn strspn
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>

void main()
  {
    JSTRING str;
.exmp break
    str = jstrmatch( "Gone for lunch", "aeiou" );
    if( str != NULL )
        printf( "%s\n", str );
    else
        printf( "Not found\n" );
    str = jstrmatch( "Gone for lunch", "xyz" );
    if( str != NULL )
        printf( "%s\n", str );
    else
        printf( "Not found\n" );
  }
.exmp output
one for lunch
Not found
.exmp end
.class WATCOM
.system
