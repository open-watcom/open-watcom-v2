.func jstrcat _fjstrcat
.synop begin
#include <jstring.h>
JSTRING jstrcat( JCHAR *dst, const JCHAR *src );
.if &farfnc eq 1 .do begin
FJSTRING __far _fjstrcat( JCHAR __far *dst,
                          const JCHAR __far *src );
.do end
.ixfunc2 '&Jstring' &func
.ixfunc2 '&Jconcat' &func
.ixfunc2 '&Jstring' &ffunc
.ixfunc2 '&Jconcat' &ffunc
.synop end
.desc begin
.if &farfnc eq 0 .do begin
The &func function appends
.do end
.el .do begin
The &func and &ffunc functions append
.do end
a copy of the Kanji string pointed to by
.arg src
(including the terminating null character)
to the end of the Kanji string pointed to by
.arg dst
.ct .li .
The first character of
.arg src
overwrites the null character at the end of
.arg dst
.ct .li .
.im ffarfunc
.desc end
.return begin
The value of
.arg dst
is returned.
.return end
.see begin
.seelist jstrcat jstrncat strcat strncat
.see end
.exmp begin
#include <stdio.h>
#include <string.h>
#include <jstring.h>

void main()
  {
    JCHAR buffer[80];
.exmp break
    strcpy( buffer, "Hello " );
    jstrcat( buffer, "world" );
    printf( "%s\n", buffer );
  }
.exmp output
Hello world
.exmp end
.class WATCOM
.system
