.func jstrset _fjstrset
.synop begin
#include <jstring.h>
JSTRING jstrset( JCHAR *s1, JMOJI fill );
.ixfunc2 '&Jstring' &funcb
.if &farfnc eq 1 .do begin
FJSTRING __far _fjstrset( JCHAR __far *s1, JMOJI fill );
.ixfunc2 '&Jstring' &ffunc
.do end
.synop end
.desc begin
.if &farfnc eq 0 .do begin
The
.id &funcb.
function fills
.do end
.el .do begin
The
.id &funcb.
and
.id &ffunc.
functions fill
.do end
the string pointed to by
.arg s1
with the value of the argument
.arg fill
.ct .li .
If
.arg fill
is a single-byte character, its value must be in the low-order byte
of the 16-bit value and the high-order byte must be zero.
If
.arg fill
is a double-byte character,
the first byte of the double-byte character must be in the high-order
byte of the 16-bit value
and the second byte of the double-byte character must be in the
low-order byte of the 16-bit value.
.np
If
.arg fill
is the null character, then both high- and low-order bytes must
be null.
.np
When the length of the string is odd and the fill character is a
double-byte character, then the last character in the string is set to
ASCII blank (hex '20').
.np
The terminating null character in the original string remains
unchanged.
.im ffarfunc
.desc end
.return begin
The address of the original string
.arg s1
is returned.
.return end
.see begin
.seelist jstrnset jstrset strnset strset
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>

JCHAR source[] = { "A sample STRING" };

void main()
  {
    printf( "%s\n", source );
    printf( "%s\n", jstrset( source, '=' ) );
    printf( "%s\n", jstrset( source, '*' ) );
  }
.exmp output
A sample STRING
===============
***************
.exmp end
.class WATCOM
.system
