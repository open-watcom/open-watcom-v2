.ix 'floating-point' 'number'
.ix 'type' 'floating-point'
.pp
A floating-point number is a number which may contain a decimal point and
digits following the decimal point.
The range of floating-point numbers is
usually considerably larger than that of integers, but the efficiency of
integers is usually much greater.
Integers are always exact quantities,
whereas floating-point numbers sometimes suffer from round-off error and
loss of precision.
.pp
On some computers, floating-point arithmetic is
.us emulated
(simulated)
by software,
rather than hardware.
.ix 'floating-point' 'emulation'
Software emulation can greatly reduce the speed
of a program.
While this should not affect the portability of
a program,
a prudent programmer limits the use of floating-point
numbers.
.pp
There are three floating-point number types,
.ix 'type' 'float'
.kw float
..ct ,
.ix 'type' 'double'
.kw double
..ct ,
and
.ix 'type' 'long double'
.kw long double
..ct ..li .
.pp
The appendix "&numlimit."
discusses a set of macro definitions
describing the range and other characteristics
of the various numeric types.
The macros from the header
.hdr <float.h>
..ct ,
which describe the floating-point types,
are discussed.
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.pp
.shade begin
The following table gives the ranges available on the 80x86/80x87
using the
&wcboth. compiler.
The floating-point format is the
.us IEEE Standard for Binary Floating-Point Arithmetic
(ANSI/IEEE Std 754-1985).
.shade end
.cp 12
.shade begin
..im float87
.shade end
.pp
.cp 7
.shade begin
By default, the &wcboth. compilers
.ix 'emulation' 'floating-point'
.ix 'floating-point' 'emulation'
emulate floating-point arithmetic.
If the
8087 or 80x87 Numeric Processor Extension
.ix 'math chip'
.ix 'math coprocessor'
.ix 'numeric coprocessor'
(numeric coprocessor, math chip)
will be present at execution time,
the compiler can be forced to generate floating-point instructions for
the coprocessor
by specifying a command line switch, as described in the &userguide..
Other than an improvement in
execution speed, the final result should be the same as if
the processor is not present.
.shade end
..do end
..if '&target' eq 'PC 370' ..th ..do begin
.shade begin
.pp
The following table gives the ranges normally available on
an IBM System/370 model computer.
..im float370
.shade end
..do end
.*
.************************************************************************
.*
.keep begin
.pp
The following are some examples of declarations of objects with
floating-point type:
.millust begin
float       a;
double      b;
long double c;
.millust end
.keep end
