.func strtoimax wcstoimax _ustrtoimax
#include <inttypes.h>
intmax_t strtoimax( const char *ptr,
                    char **endptr,
                    int base );
.ixfunc2 '&Conversion' &func
.if &'length(&wfunc.) ne 0 .do begin
intmax_t wcstoimax( const wchar_t *ptr,
                    wchar_t **endptr,
                    int base );
.ixfunc2 '&Conversion' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&ufunc.) ne 0 .do begin
intmax_t _ustrtoimax( const wchar_t *ptr,
                      wchar_t **endptr,
                      int base );
.ixfunc2 '&Conversion' &ufunc
.do end
.funcend
.desc begin
The &func function converts the string pointed to by
.arg ptr
to an object of type
.id intmax_t.
The &func function recognizes a string containing:
.begbull $compact
.bull
optional white space,
.bull
an optional plus or minus sign,
.bull
a sequence of digits and letters.
.endbull
.np
The conversion ends at the first unrecognized character.
A pointer to that character will be stored in the object to which
.arg endptr
points if
.arg endptr
is not
.mono NULL.
.np
If
.arg base
is zero, the first characters after the optional sign determine the
base used for the conversion.
If the first characters are "0x" or "0X" the digits are treated as
hexadecimal.
If the first character is '0', the digits are treated as octal.
Otherwise the digits are treated as decimal.
.np
If
.arg base
is not zero, it must have a value between 2 and 36.
The letters a-z and A-Z represent the values 10 through 35.
Only those letters whose designated values are less than
.arg base
are permitted.
If the value of
.arg base
is 16, the characters "0x" or "0X" may optionally precede the sequence
of letters and digits.
.im widefunc
.desc end
.return begin
The &func function returns the converted value.
If the correct value would cause overflow,
.kw INTMAX_MAX
or
.kw INTMAX_MIN
is returned according to the sign, and
.kw errno
is set to
.kw ERANGE
.ct .li .
If
.arg base
is out of range, zero is returned and
.kw errno
is set to
.kw EDOM
.ct .li .
.return end
.see begin
.im seestoi &function.
.see end
.exmp begin
#include <inttypes.h>
#include <stdlib.h>

void main()
{
    intmax_t v;
.exmp break
    v = strtoimax( "12345678909876", NULL, 10 );
}
.exmp end
.class ANSI
.system
