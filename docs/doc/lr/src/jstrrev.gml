.func jstrrev _fjstrrev
.synop begin
#include <jstring.h>
JSTRING jstrrev( JCHAR *s1 );
.ixfunc2 '&Jstring' &func
.if &farfnc eq 1 .do begin
FJSTRING __far _fjstrrev( JCHAR __far *s1 );
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
the Kanji string
.arg s1
with a Kanji string whose single- or double-byte characters are in the
reverse order.
.im ffarfunc
.desc end
.return begin
The address of the original string
.arg s1
is returned.
.return end
.see begin
.seelist &function. jstrrev strrev
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>

JCHAR source[] = { "A sample STRING" };

void main()
  {
    printf( "%s\n", source );
    printf( "%s\n", jstrrev( source ) );
    printf( "%s\n", jstrrev( source ) );
  }
.exmp output
A sample STRING
GNIRTS elpmas A
A sample STRING
.exmp end
.class WATCOM
.system
