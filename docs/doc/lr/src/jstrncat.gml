.func jstrncat _fjstrncat
.synop begin
#include <jstring.h>
JSTRING jstrncat( JCHAR *dst, const JCHAR *src, size_t n );
.ixfunc2 '&String' &funcb
.ixfunc2 '&Concats' &funcb
.if &farfnc ne 0 .do begin
FJSTRING _fjstrncat( JCHAR __far *dst, const JCHAR __far *src, size_t n );
.ixfunc2 '&String' &ffunc
.ixfunc2 '&Concats' &ffunc
.do end
.synop end
.desc begin
.if &farfnc eq 0 .do begin
The
.id &funcb.
function appends
.do end
.el .do begin
The
.id &funcb.
and
.id &ffunc.
functions append
.do end
not more than
.arg n
single or double-byte characters of the Kanji string pointed to by
.arg src
to the end of the Kanji string pointed to by
.arg dst
.period
The first character of
.arg src
overwrites the null character at the end of
.arg dst
.period
A terminating null character is always appended to the result.
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
the value of
.arg dst
.period
.return end
.see begin
.seelist jstrcat jstrncat strcat strncat
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
