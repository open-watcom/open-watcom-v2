.func jstrlwr _fjstrlwr
.synop begin
#include <jstring.h>
JSTRING jstrlwr( JCHAR *s1 );
.ixfunc2 '&Jstring' &func
.if &farfnc eq 1 .do begin
FJSTRING __far _fjstrlwr( JCHAR __far *s1 );
.ixfunc2 '&Jstring' &ffunc
.do end
.synop end
.desc begin
.if &farfnc eq 0 .do begin
The
.id &func.
function replaces
.do end
.el .do begin
The
.id &func.
and
.id &ffunc.
functions replace
.do end
all uppercase Roman characters ("A" to "Z") in the Kanji string
.arg s1
with lowercase characters.
.im ffarfunc
.desc end
.return begin
The address of the original string
.arg s1
is returned.
.return end
.see begin
.seelist &function. jstrlwr jstrupr jtolower jtoupper strlwr strupr
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>

JCHAR source[] = { "A mixed-case STRING" };

void main()
  {
    printf( "%s\n", source );
    printf( "%s\n", jstrlwr( source ) );
    printf( "%s\n", source );
  }
.exmp output
A mixed-case STRING
a mixed-case string
a mixed-case string
.exmp end
.class WATCOM
.system
