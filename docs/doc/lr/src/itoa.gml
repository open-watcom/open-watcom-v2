.func itoa _itoa _itow
.synop begin
.funcw _itow
#include <stdlib.h>
char *itoa( int value, char *buffer, int radix );
.ixfunc2 '&Conversion' &funcb
.if &'length(&_func.) ne 0 .do begin
char *_itoa( int value, char *buffer, int radix );
.ixfunc2 '&Conversion' &_func
.do end
.if &'length(&wfunc.) ne 0 .do begin
wchar_t *_itow( int value, wchar_t *buffer,
                int radix );
.ixfunc2 '&Conversion' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The
.id &funcb.
function converts the binary integer
.arg value
into the equivalent string in base
.arg radix
notation storing the result in the character array pointed to by
.arg buffer
.ct .li .
A null character is appended to the result.
The size of
.arg buffer
must be at least (8 * sizeof(int) + 1) bytes when converting values in
base 2.
That makes the size 17 bytes on 16-bit machines, and 33 bytes on
32-bit machines.
The value of
.arg radix
must satisfy the condition:
.millust begin
2 <= radix <= 36
.millust end
If
.arg radix
is 10 and
.arg value
is negative, then a minus sign is prepended to the result.
.if &'length(&_func.) ne 0 .do begin
.np
The
.id &_func.
function is identical to &funcb..
Use
.id &_func.
for ISO C naming conventions.
.do end
.if &'length(&wfunc.) ne 0 .do begin
.np
The
.id &wfunc.
function is identical to
.id &funcb.
except that it
produces a wide-character string (which is twice as long).
.do end
.desc end
.return begin
The
.id &funcb.
function returns the pointer to the result.
.return end
.see begin
.im seestoi
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>

void main()
{
    char buffer[20];
    int base;
.exmp break
    for( base = 2; base <= 16; base = base + 2 )
        printf( "%2d %s\n", base,
                itoa( 12765, buffer, base ) );
}
.exmp output
 2 11000111011101
 4 3013131
 6 135033
 8 30735
10 12765
12 7479
14 491b
16 31dd
.exmp end
.ansiname &_func
.class WATCOM
.system
