.func jstrncpy _fjstrncpy
.synop begin
#include <jstring.h>
JSTRING jstrncpy( JCHAR *dst,
                  const JCHAR *src,
                  size_t n );
.ixfunc2 '&Jstring' &func
.ixfunc2 '&Jcopy' &func
.if &farfnc eq 1 .do begin
FJSTRING __far _fjstrncpy( JCHAR __far *dst,
                           const JCHAR __far *src,
                           size_t n );
.ixfunc2 '&Jstring' &ffunc
.ixfunc2 '&Jcopy' &ffunc
.do end
.synop end
.desc begin
.if &farfnc eq 0 .do begin
The
.id &func.
function copies
.do end
.el .do begin
The
.id &func.
and
.id &ffunc.
functions copy
.do end
no more than
.arg n
single- or double-byte characters from the Kanji string pointed to by
.arg src
into the array pointed to by
.arg dst
.ct .li .
Copying of overlapping objects is not guaranteed to work properly.
See the
.kw memmove
function if you wish to copy objects that overlap.
.pp
If the Kanji string pointed to by
.arg src
is shorter than
.arg n
characters, null characters are appended to the copy in the array pointed
to by
.arg dst
.ct , until
.arg n
characters in all have been written.
If the Kanji string pointed to by
.arg src
is longer than
.arg n
single- or double-byte characters, then the result will not be
terminated by a null character.
.im ffarfunc
.desc end
.return begin
.if &farfnc eq 0 .do begin
The
.id &func.
function returns
.do end
.el .do begin
The
.id &func.
and
.id &ffunc.
functions return
.do end
the value of
.arg dst
.ct .li .
.return end
.see begin
.seelist jstrncpy strcpy strdup strncpy
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>

void main()
  {
    JCHAR buffer[15];
.exmp break
    printf( "%s\n", jstrncpy( buffer, "abcdefg", 10 ) );
    printf( "%s\n", jstrncpy( buffer, "1234567",  6 ) );
    printf( "%s\n", jstrncpy( buffer, "abcdefg",  3 ) );
    printf( "%s\n", jstrncpy( buffer, "*******",  0 ) );
  }
.exmp output
abcdefg
123456g
abc456g
abc456g
.exmp end
.class WATCOM
.system
