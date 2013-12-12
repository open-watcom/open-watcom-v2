.func ecvt _ecvt _wecvt
.synop begin
#include <stdlib.h>
char *ecvt( double value,
            int ndigits,
            int *dec,
            int *sign );
.ixfunc2 '&Conversion' &func
.if &'length(&_func.) ne 0 .do begin
char *_ecvt( double value,
             int ndigits,
             int *dec,
             int *sign );
.ixfunc2 '&Conversion' &_func
.do end
.if &'length(&wfunc.) ne 0 .do begin
wchar_t *_wecvt( double value,
                 int ndigits,
                 int *dec,
                 int *sign );
.ixfunc2 '&Conversion' &wfunc
.do end
.synop end
.desc begin
The &func function converts the floating-point number
.arg value
into a character string.
The parameter
.arg ndigits
specifies the number of significant digits desired.
The converted number will be rounded to
.arg ndigits
of precision.
.np
The character string will contain only digits and is terminated by
a null character.
The integer pointed to by
.arg dec
will be filled in with a value indicating the position of the decimal
point relative to the start of the string of digits.
A zero or negative value indicates that the decimal point lies to the
left of the first digit.
The integer pointed to by
.arg sign
will contain 0 if the number is positive, and non-zero if the number
is negative.
.if &'length(&_func.) ne 0 .do begin
.np
The &_func function is identical to &func..
Use &_func for ANSI/ISO naming conventions.
.do end
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is identical to &func except that it produces a
wide-character string.
.do end
.desc end
.return begin
The &func function returns a pointer to a static buffer containing the
converted string of digits.
Note:
&func and
.kw fcvt
both use the same static buffer.
.return end
.see begin
.seelist ecvt fcvt gcvt printf
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>

void main()
  {
     char *str;
     int  dec, sign;
.exmp break
     str = ecvt( 123.456789, 6, &dec, &sign );
     printf( "str=%s, dec=%d, sign=%d\n", str,dec,sign );
  }
.exmp output
str=123457, dec=3, sign=0
.exmp end
.class begin WATCOM
.ansiname &_func
.class end
.system
