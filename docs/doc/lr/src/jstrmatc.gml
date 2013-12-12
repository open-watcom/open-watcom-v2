.func jstrmatch _fjstrmatch
#include <jstring.h>
JSTRING jstrmatch( const JCHAR *str, const JCHAR *charset );
.ixfunc2 '&Jstring' &func
.ixfunc2 '&Jsearch' &func
.if &farfnc eq 1 .do begin
FJSTRING __far _fjstrmatch( const JCHAR __far *str,
                            const JCHAR __far *charset );
.ixfunc2 '&Jstring' &ffunc
.ixfunc2 '&Jsearch' &ffunc
.do end
.synop end
.desc begin
.if &farfnc eq 0 .do begin
The &func function locates
.do end
.el .do begin
The &func and &ffunc functions locate
.do end
the first occurrence in the Kanji string pointed to by
.arg str
of a single- or double-byte character that appears
in the Kanji string pointed to by
.arg charset
.ct .li .
.im ffarfunc
.desc end
.return begin
.if &farfnc eq 0 .do begin
The &func function returns
.do end
.el .do begin
The &func and &ffunc functions return
.do end
a pointer to the located character, or
.mono NULL
if no character is found.
.return end
.see begin
.seelist &function. jstrmatch jstrskip jstrspn strspn
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
