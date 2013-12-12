.func strtod wcstod
.synop begin
.funcw wcstod
#include <stdlib.h>
double strtod( const char *ptr, char **endptr );
.ixfunc2 '&Conversion' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
double wcstod( const wchar_t *ptr, wchar_t **endptr );
.ixfunc2 '&Conversion' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.*
.desc begin
The &func function converts the string pointed to by
.arg ptr
to
.id double
representation.
First, it decompose the input string into three parts: an initial, possibly
empty, sequence of white-space characters (as specified by the
.mono isspace
function), a subject sequence resembling a floating-point constant or
representing an infinity or NaN; and a final string of one or more
unrecognized characters, including the terminating null character of the
input string. Then, it attempts to convert the subject sequence to a
floating-point number, and return the result.
.np
The expected form of the subject sequence is an optional plus or minus sign,
then one of the following:
.begbull $compact
.bull
a decimal floating-point number
.bull
a hexadecimal floating-point number
.bull
.mono INF
or
.mono INFINITY
.ct , ignoring case
.bull
.mono NAN
.ct , ignoring case, optionally followed by a sequence of digits and nondigits
(upper- or lowercase characters or underscore) enclosed in parentheses.
.endbull
.np
The subject sequence is defined as the longest initial subsequence of the
input string, starting with the first non-whitespace character, that is of
the expected form. The subject sequence contains no characters if the input
string is not of the expected form.
.np
A decimal floating-point number recognized by &func (after optional sign was
processed) is a string containing:
.begbull $compact
.bull
a sequence of digits containing an optional decimal point,
.bull
an optional 'e' or 'E' followed by an optionally signed sequence of
digits.
.endbull
.np
A hexadecimal floating-point number recognized by &func (after optional sign
was processed) is a string containing:
.begbull $compact
.bull
a
.mono 0X
prefix, ignoring case,
.bull
a sequence of hexadecimal digits containing an optional decimal point,
.bull
an optional 'p' or 'P' followed by an optionally signed sequence of decimal
digits.
.endbull
.np
The subject sequence is defined as the longest initial subsequence of the
input string, starting with the first non-white-space character, that is of
the expected form. The subject sequence contains no characters if the input
string is not of the expected form.
.np
If the subject sequence contains
.mono NAN
.'ct , a NaN (with appropriate sign) will be returned; the optional
digit-nondigit sequence is ignored. If the subject sequence contains
.mono INF
.ct , the value of infinity (with appropriate sign) will be returned.
This case can be distinguished from overflow by checking
.kw errno
.ct .li .
.np
For a hexadecimal floating-point number, the optional exponent is
binary (that is, denotes a power of two), not decimal.
.np
A pointer to the final string (following the subject sequence) will be stored
in the object to which
.arg endptr
points if
.arg endptr
is not
.mono NULL.
By comparing the "end" pointer with
.arg ptr
.ct , it can be determined how much of the string, if any, was scanned by
the &func function.
.im widefunc
.desc end
.*
.return begin
The &func function returns the converted value, if any. If no conversion
could be performed, zero is returned.
If the correct value would cause overflow, plus or minus
.kw HUGE_VAL
is returned according to the sign, and
.kw errno
is set to
.kw ERANGE
.ct .li .
If the correct value would cause underflow, then zero is returned, and
.kw errno
is set to
.kw ERANGE
.ct .li .
.im errnocnv
.return end
.*
.see begin
.seelist strtod atof
.see end
.*
.exmp begin
#include <stdio.h>
#include <stdlib.h>

void main( void )
{
    double pi;
.exmp break
    pi = strtod( "3.141592653589793", NULL );
    printf( "pi=%17.15f\n",pi );
}
.exmp end
.class ISO C90
.system
