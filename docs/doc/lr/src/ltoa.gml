.func ltoa _ltoa _ltow
.funcw _ltow
#include <stdlib.h>
char *ltoa( long int value,
            char *buffer,
            int radix );
.ixfunc2 '&Conversion' &func
.if &'length(&_func.) ne 0 .do begin
char *_ltoa( long int value,
             char *buffer,
             int radix );
.ixfunc2 '&Conversion' &_func
.do end
.if &'length(&wfunc.) ne 0 .do begin
wchar_t *_ltow( long int value,
                wchar_t *buffer,
                int radix );
.ixfunc2 '&Conversion' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The &func function converts the binary integer
.arg value
into the equivalent string in base
.arg radix
notation storing the result in the character array pointed to by
.arg buffer
.ct .li .
A null character is appended to the result.
The size of
.arg buffer
must be at least 33 bytes when converting values in base 2.
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
The &_func function is identical to &func..
Use &_func for ANSI/ISO naming conventions.
.do end
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is identical to &func except that it produces a
wide-character string (which is twice as long).
.do end
.desc end
.return begin
The &func function returns a pointer to the result.
.return end
.see begin
.im seestoi &function.
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>

void print_value( long value )
{
    int base;
    char buffer[33];

    for( base = 2; base <= 16; base = base + 2 )
        printf( "%2d %s\n", base,
                ltoa( value, buffer, base ) );
}

void main()
{
    print_value( 12765L );
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
.class begin WATCOM
.ansiname &_func
.class end
.system
