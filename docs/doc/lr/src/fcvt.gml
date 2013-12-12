.func fcvt _fcvt _wfcvt
.synop begin
#include <stdlib.h>
char *fcvt( double value,
            int ndigits,
            int *dec,
            int *sign );
.ixfunc2 '&Conversion' &func
.if &'length(&_func.) ne 0 .do begin
char *_fcvt( double value,
             int ndigits,
             int *dec,
             int *sign );
.ixfunc2 '&Conversion' &_func
.do end
.if &'length(&wfunc.) ne 0 .do begin
wchar_t *_wfcvt( double value,
                 int ndigits,
                 int *dec,
                 int *sign );
.ixfunc2 '&Conversion' &wfunc
.do end
.synop end
.desc begin
The
.id &func.
function converts the floating-point number
.arg value
into a character string.
The parameter
.arg ndigits
specifies the number of digits desired after the decimal point.
The converted number will be rounded to this position.
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
The
.id &_func.
function is identical to &func..
Use
.id &_func.
for ANSI/ISO naming conventions.
.do end
.if &'length(&wfunc.) ne 0 .do begin
.np
The
.id &wfunc.
function is identical to
.id &func.
except that it produces a
wide-character string.
.do end
.desc end
.return begin
The
.id &func.
function returns a pointer to a static buffer containing the
converted string of digits.
Note:
.kw ecvt
and
.id &func.
both use the same static buffer.
.return end
.see begin
.seelist fcvt ecvt gcvt printf
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>

void main()
  {
     char *str;
     int  dec, sign;
.exmp break
     str = fcvt( -123.456789, 5, &dec, &sign );
     printf( "str=%s, dec=%d, sign=%d\n", str,dec,sign );
  }
.exmp output
str=12345679, dec=3, sign=-1
.exmp end
.class begin WATCOM
.ansiname &_func
.class end
.system
