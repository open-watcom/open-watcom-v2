.func jstrskip _fjstrskip
.synop begin
#include <jstring.h>
JSTRING jstrskip( const JCHAR *str,
                  const JCHAR *charset );
.ixfunc2 '&Jstring' &funcb
.ixfunc2 '&Jsearch' &funcb
.if &farfnc eq 1 .do begin
FJSTRING __far _fjstrskip( const JCHAR __far *str,
                           const JCHAR __far *charset );
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
.arg str
of a single- or double-byte character that does not appear
in the Kanji string pointed to by
.arg charset
.ct .li .
.im ffarfunc
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
    str = jstrskip( "Gone for lunch", "aeiou" );
    if( str != NULL )
        printf( "%s\n", str );
    else
        printf( "Not found\n" );
    str = jstrskip( "Cat crazy", "ABCDEFabcdef" );
    if( str != NULL )
        printf( "%s\n", str );
    else
        printf( "Not found\n" );
  }
.exmp output
Gone for lunch
t crazy
.exmp end
.class WATCOM
.system
