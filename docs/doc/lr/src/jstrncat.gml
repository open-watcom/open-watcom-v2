.func jstrncat _fjstrncat
#include <jstring.h>
JSTRING jstrncat( JCHAR *dst,
                  const JCHAR *src,
                  size_t n );
.ixfunc2 '&String' &func
.ixfunc2 '&Concats' &func
.if &farfnc eq 1 .do begin
FJSTRING __far _fjstrncat( JCHAR __far *dst,
                           const JCHAR __far *src,
                           size_t n );
.ixfunc2 '&String' &ffunc
.ixfunc2 '&Concats' &ffunc
.do end
.synop end
.desc begin
.if &farfnc eq 0 .do begin
The &func function appends
.do end
.el .do begin
The &func and &ffunc functions append
.do end
not more than
.arg n
single or double-byte characters of the Kanji string pointed to by
.arg src
to the end of the Kanji string pointed to by
.arg dst
.ct .li .
The first character of
.arg src
overwrites the null character at the end of
.arg dst
.ct .li .
A terminating null character is always appended to the result.
.im ffarfunc
.desc end
.return begin
.if &farfnc eq 0 .do begin
The &func function returns
.do end
.el .do begin
The &func and &ffunc functions return
.do end
the value of
.arg dst
.ct .li .
.return end
.see begin
.seelist &function. jstrcat jstrncat strcat strncat
.see end
.exmp begin
#include <stdio.h>
#include <string.h>
#include <jstring.h>

JCHAR buffer[80];

void main()
  {
    strcpy( buffer, "Hello " );
    jstrncat( buffer, "world", 8 );
    printf( "%s\n", buffer );
    jstrncat( buffer, "*************", 4 );
    printf( "%s\n", buffer );
  }
.exmp output
Hello world
Hello world****
.exmp end
.class WATCOM
.system
