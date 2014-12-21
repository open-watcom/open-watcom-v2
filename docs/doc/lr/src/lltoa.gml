.func lltoa _lltoa _lltow
.synop begin
#include <stdlib.h>
char *lltoa( long long int value,
             char *buffer,
             int radix );
.ixfunc2 '&Conversion' &funcb
.if &'length(&_func.) ne 0 .do begin
char *_lltoa( long long int value,
              char *buffer,
              int radix );
.ixfunc2 '&Conversion' &_func
.do end
.if &'length(&wfunc.) ne 0 .do begin
wchar_t *_lltow( long long int value,
                 wchar_t *buffer,
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
must be at least 65 bytes when converting values in base 2.
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
.im ansiconf
.if &'length(&wfunc.) ne 0 .do begin
.np
The
.id &wfunc.
function is identical to
.id &funcb.
except that it produces a
wide-character string (which is twice as long).
.do end
.desc end
.return begin
The
.id &funcb.
function returns a pointer to the result.
.return end
.see begin
.im seestoi
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>

void print_value( long value )
{
    int base;
    char buffer[65];

    for( base = 2; base <= 16; base = base + 2 )
        printf( "%2d %s\n", base,
                lltoa( value, buffer, base ) );
}

void main()
{
    print_value( 1234098765LL );
}
.exmp output
 2 1001001100011101101101001001101
 4 1021203231221031
 6 322243004113
 8 11143555115
10 1234098765
12 2a5369639
14 b9c8863b
16 498eda4d
.exmp end
.class WATCOM
.system
