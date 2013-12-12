.func jstrupr _fjstrupr
.synop begin
#include <jstring.h>
JSTRING jstrupr( JCHAR *s1 );
.ixfunc2 '&Jstring' &func
.if &farfnc eq 1 .do begin
FJSTRING __far _fjstrupr( JCHAR __far *s1 );
.ixfunc2 '&Jstring' &ffunc
.do end
.synop end
.desc begin
.if &farfnc eq 0 .do begin
The &func function replaces
.do end
.el .do begin
The &func and &ffunc functions replace
.do end
all lowercase Roman characters ("a" to "z") in the Kanji string
.arg s1
with uppercase characters.
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
    printf( "%s\n", jstrupr( source ) );
    printf( "%s\n", source );
  }
.exmp output
A mixed-case STRING
A MIXED-CASE STRING
A MIXED-CASE STRING
.exmp end
.class WATCOM
.system
