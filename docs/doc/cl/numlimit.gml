..if '&format' eq '7x9' ..th ..do begin
.  ..sr numboxcol1=17
.  ..sr numboxcol2=22
..do end
..el ..do begin
.  ..sr numboxcol1=19
.  ..sr numboxcol2=27
..do end
.*
.dm numboxbeg begin
.  ..br
.  .if &e'&nobox eq 0 .do begin
.  .boxdef
.  .boxcol &numboxcol1.
.  .boxcol &numboxcol2.
.  .boxbeg
$Macro: :SF font=4.&*:eSF.$Value
.  .do end
.  .el .do begin
.  .millust begin
Macro: &*       Value
.  .do end
.  .boxline
.dm numboxbeg end
.*
.dm numterm begin
.  .if &e'&nobox eq 0 .do begin
$&*1.$:SF font=4.&*2:eSF.
.  .do end
.  .el .do begin
&*1             &*2
.  .do end
.dm numterm end
.*
.dm ansi begin
.  .numterm 'ISO' '&*'
.dm ansi end
.*
.dm noansi begin
.  .if &e'&nobox eq 0 .do begin
$ISO$no value specified
.  .do end
.  .el .do begin
ISO             no value specified
.  .do end
.dm noansi end
.*
.dm watcomc begin
.  ..if '&target' eq 'PC'  or  '&target' eq 'PC 370' ..th ..do begin
.  .  .numterm '&wc286.' &*1
.  ..do end
.dm watcomc end
.*
.dm c386 begin
.  ..if '&target' eq 'PC'  or  '&target' eq 'PC 370' ..th ..do begin
.  .  .numterm '&wc386.' &*1
.  ..do end
.dm c386 end
.*
.dm bothc begin
.  ..if '&target' eq 'PC'  or  '&target' eq 'PC 370' ..th ..do begin
.  .  .numterm '&wcboth.' &*1
.  ..do end
.dm bothc end
.*
.dm wlooc begin
.  ..if '&target' eq 'PC 370'  or  '&target' eq '370' ..th ..do begin
.  .  .numterm '&wlooc.' &*1
.  ..do end
.dm wlooc end
.*
.dm numboxend begin
.  .if &e'&nobox eq 0 .do begin
.  .boxend
.  .do end
.  .el .do begin
.  .millust end
.  .do end
.dm numboxend end
.*
.ix 'numerical limits'
.ix 'macro' 'numerical limits'
.pp
Although the various numerical types may have different ranges depending
on the implementation of the C compiler, it is still possible to write
programs that can adapt to these changing ranges. In most
circumstances, it is clear whether an integer object is sufficiently
large to contain all necessary values for it, regardless of whether
or not the integer is only 16 bits.
.pp
However, a programmer may want to be able to conditionally compile
code based on information about the range of certain types.
The header
.hdr <limits.h>
defines a set of macros that describe the range of the various
integer types.
The header
.hdr <float.h>
defines another set of macros that describe the range and other
characteristics of the various floating-point types.
.*
.section Numerical Limits for Integer Types
.*
.ix 'numerical limits' 'integer'
.ix 'integer' 'limits'
.pp
The following macros are replaced by constant expressions that may be
used in
.kwpp #if
preprocessing directives.
For a compiler to conform to the C language standard,
the magnitude of the value of the expression
provided by the compiler
must equal or exceed the ISO value given below, and have the same sign.
(Positive values must be greater than or equal to the ISO value.
Negative values must be less than or equal to the ISO value.)
.* In the following pages, the ISO value is the
.* .us smallest
.* maximum value that the compiler may impose and still conform to the
.* ISO standard.
The values for the actual compilers are shown following the ISO value.
.begbull
.keep begin
.bull
the number of bits in the smallest object that is not a bit-field
(byte)
.numboxbeg CHAR_BIT
.ansi >= 8
.bothc 8
.wlooc 8
.numboxend
.keep break
.bull
the minimum value for an object of type
.kw signed char
.numboxbeg SCHAR_MIN
.ansi  <= -127
.bothc -128
.wlooc -128
.numboxend
.keep break
.bull
the maximum value for an object of type
.kw signed char
.numboxbeg SCHAR_MAX
.ansi  >= 127
.bothc 127
.wlooc 127
.numboxend
.keep break
.bull
the maximum value for an object of type
.kw unsigned char
.numboxbeg UCHAR_MAX
.ansi  >= 255
.bothc 255
.wlooc 255
.numboxend
.keep break
.bull
the minimum value for an object of type
.kw char
.pp
If
.kw char
is
.kw unsigned
(the default case)
.numboxbeg CHAR_MIN
.ansi  0
.bothc 0
.wlooc 0
.numboxend
.keep break
.pc
If
.kw char
is
.kw signed
(by using the command-line switch to force it to be signed),
then
.mono CHAR_MIN
is equivalent to
.mono SCHAR_MIN
.numboxbeg CHAR_MIN
.ansi  <= -127
.bothc -128
.wlooc -128
.numboxend
.keep break
.bull
the maximum value for an object of type
.kw char
.pp
If
.kw char
is
.kw unsigned
(the default case),
then
.mono CHAR_MAX
is equivalent to
.mono UCHAR_MAX
.numboxbeg CHAR_MAX
.ansi  >= 255
.bothc 255
.wlooc 255
.numboxend
.keep break
.pc
If
.kw char
is
.kw signed
(by using the command-line switch to force it to be signed),
then
.mono CHAR_MAX
is equivalent to
.mono SCHAR_MAX
.numboxbeg CHAR_MAX
.ansi  >= 127
.bothc 127
.wlooc 127
.numboxend
.keep break
.bull
the maximum number of bytes in a multibyte character, for any supported locale
.numboxbeg MB_LEN_MAX
.ansi  >= 1
.bothc 2
.wlooc 1
.numboxend
.keep break
.bull
the minimum value for an object of type
.kw short int
.numboxbeg SHRT_MIN
.ansi  <= -32767
.bothc -32768
.wlooc -32768
.numboxend
.keep break
.bull
the maximum value for an object of type
.kw short int
.numboxbeg SHRT_MAX
.ansi  >= 32767
.bothc 32767
.wlooc 32767
.numboxend
.keep break
.bull
the maximum value for an object of type
.kw unsigned short int
.numboxbeg USHRT_MAX
.ansi  >= 65535
.bothc 65535
.wlooc 65535
.numboxend
.keep break
.bull
the minimum value for an object of type
.kw int
.numboxbeg INT_MIN
.ansi    <= -32767
.watcomc -32768
.c386    -2147483648
.wlooc   -2147483648
.numboxend
.keep break
.bull
the maximum value for an object of type
.kw int
.numboxbeg INT_MAX
.ansi    >= 32767
.watcomc 32767
.c386    2147483647
.wlooc   2147483647
.numboxend
.keep break
.bull
the maximum value for an object of type
.kw unsigned int
.numboxbeg UINT_MAX
.ansi    >= 65535
.watcomc 65535
.c386    4294967295
.wlooc   4294967295
.numboxend
.keep break
.bull
the minimum value for an object of type
.kw long int
.numboxbeg LONG_MIN
.ansi  <= -2147483647
.bothc -2147483648
.wlooc -2147483648
.numboxend
.keep break
.bull
the maximum value for an object of type
.kw long int
.numboxbeg LONG_MAX
.ansi  >= 2147483647
.bothc 2147483647
.wlooc 2147483647
.numboxend
.keep break
.bull
the maximum value for an object of type
.kw unsigned long int
.numboxbeg ULONG_MAX
.ansi  >= 4294967295
.bothc 4294967295
.wlooc 4294967295
.numboxend
.keep break
.bull
the minimum value for an object of type
.kw long long int
.numboxbeg LLONG_MIN
.ansi  <= -9223372036854775807
.bothc -9223372036854775808
.wlooc -9223372036854775808
.numboxend
.keep break
.bull
the maximum value for an object of type
.kw long long int
.numboxbeg LLONG_MAX
.ansi  >= 9223372036854775807
.bothc 9223372036854775807
.wlooc 9223372036854775807
.numboxend
.keep break
.bull
the maximum value for an object of type
.kw unsigned long long int
.numboxbeg ULLONG_MAX
.ansi  >= 18446744073709551615
.bothc 18446744073709551615
.wlooc 18446744073709551615
.numboxend
.keep end
.endbull
.*
.section Numerical Limits for Floating-Point Types
.*
..if '&format' eq '7x9' ..th ..do begin
.  ..sr numboxcol1=19
.  ..sr numboxcol2=21
..do end
..el ..do begin
.  ..sr numboxcol1=23
.  ..sr numboxcol2=26
..do end
.*
.ix 'numerical limits' 'floating-point'
.ix 'floating-point' 'limits'
.pp
The following macros are replaced by expressions which are not
necessarily constant.
For a compiler to conform to the C language standard,
the magnitude of the value of the expression
provided by the compiler
must equal or exceed the ISO value given below, and have the same sign.
(Positive values must be greater than or equal to the ISO value.
Negative values must be less than or equal to the ISO value.)
.* In the following pages, the ISO value is the
.* .us smallest
.* maximum value that the compiler may impose and still conform to the
.* ISO standard.
The values for the actual compilers are shown following the ISO value.
Most compilers will exceed some of these values.
.pp
For those characteristics that have three different macros, the
macros that start with
.mono FLT_&SYSRB.
refer to type
.kw float
..ct ,
.mono DBL_&SYSRB.
refer to type
.kw double
and
.mono LDBL_&SYSRB.
refer to type
.kw long double
..ct ..li .
.begbull
.keep begin
.bull
the radix (base) of representation for the exponent
.numboxbeg FLT_RADIX
.ansi  >= 2
.bothc 2
.wlooc 16
.numboxend
.keep break
.bull
the precision, or number of digits in the floating-point mantissa, expressed
in terms of the
.mono FLT_RADIX
.numboxbeg FLT_MANT_DIG
.noansi
.bothc 23
.wlooc 6
.numboxend
.keep break
.numboxbeg DBL_MANT_DIG
.noansi
.bothc 52
.wlooc 14
.numboxend
.keep break
.numboxbeg LDBL_MANT_DIG
.noansi
.bothc 52
.wlooc 14
.numboxend
.keep break
.bull
the number of decimal digits of precision
.numboxbeg FLT_DIG
.ansi  >= 6
.bothc 6
.wlooc 8
.numboxend
.keep break
.numboxbeg DBL_DIG
.ansi  >= 10
.bothc 15
.wlooc 17
.numboxend
.keep break
.numboxbeg LDBL_DIG
.ansi  >= 10
.bothc 15
.wlooc 17
.numboxend
.keep break
.bull
the minimum negative integer
.us n
such that
.mono FLT_RADIX
raised to the power
.us n
..ct ,
minus 1,
is a normalized floating-point number, or,
.begbull $compact
.bull
the minimum exponent value in terms of
.mono FLT_RADIX
..ct , or,
.bull
the base
.mono FLT_RADIX
exponent for the floating-point value that is closest, but
not equal, to zero
.endbull
.numboxbeg FLT_MIN_EXP
.noansi
.bothc -127
.wlooc -65
.numboxend
.keep break
.numboxbeg DBL_MIN_EXP
.noansi
.bothc -1023
.wlooc -65
.numboxend
.keep break
.numboxbeg LDBL_MIN_EXP
.noansi
.bothc -1023
.wlooc -65
.numboxend
.keep break
.bull
the minimum negative integer
.us n
such that
10
raised to the power
.us n
is in the range of normalized floating-point numbers, or,
.begbull $compact
.bull
the base 10 exponent for the floating-point value that is
closest, but not equal, to zero
.endbull
.numboxbeg FLT_MIN_10_EXP
.ansi  <= -37
.bothc -38
.wlooc -78
.numboxend
.keep break
.numboxbeg DBL_MIN_10_EXP
.ansi  <= -37
.bothc -307
.wlooc -78
.numboxend
.keep break
.numboxbeg LDBL_MIN_10_EXP
.ansi  <= -37
.bothc -307
.wlooc -78
.numboxend
.keep break
.bull
the maximum integer
.us n
such that
.mono FLT_RADIX
raised to the power
.us n
..ct ,
minus 1,
is a representable finite floating-point number, or,
.begbull $compact
.bull
the maximum exponent value in terms of
.mono FLT_RADIX
..ct , or,
.bull
the base
.mono FLT_RADIX
exponent for the largest valid floating-point value
.endbull
.numboxbeg FLT_MAX_EXP
.noansi
.bothc 127
.wlooc 62                      62
.numboxend
.keep break
.numboxbeg DBL_MAX_EXP
.noansi
.bothc 1023
.wlooc 62
.numboxend
.keep break
.numboxbeg LDBL_MAX_EXP
.noansi
.bothc 1023
.wlooc 62
.numboxend
.keep break
.bull
the maximum integer
.us n
such that
10
raised to the power
.us n
is a representable finite floating-point number, or,
.begbull $compact
.bull
the base 10 exponent for the largest valid floating-point value
.endbull
.numboxbeg FLT_MAX_10_EXP
.ansi  >= 37
.bothc 38
.wlooc 75
.numboxend
.keep break
.numboxbeg DBL_MAX_10_EXP
.ansi  >= 37
.bothc 308
.wlooc 75
.numboxend
.keep break
.numboxbeg LDBL_MAX_10_EXP
.ansi  >= 37
.bothc 308
.wlooc 75
.numboxend
.keep break
.bull
the maximum representable finite floating-point number
.numboxbeg FLT_MAX
.ansi  >= 1E+37
.bothc 3.402823466E+38
.wlooc 7.2370051E+75
.numboxend
.keep break
.numboxbeg DBL_MAX
.ansi  >= 1E+37
.bothc 1.79769313486231560E+308
.wlooc 7.2370055773322608E+75
.numboxend
.keep break
.numboxbeg LDBL_MAX
.ansi  >= 1E+37
.bothc  1.79769313486231560E+308
.wlooc 7.2370055773322608E+75
.numboxend
.keep break
.bull
the difference between
.mono 1.0
and the least value greater than
.mono 1.0
that is representable in the given floating-point type, or,
.begbull $compact
.bull
the smallest number
.mono eps
such that
.mono (1.0 + eps) != 1.0
.endbull
.numboxbeg FLT_EPSILON
.ansi  <= 1E-5
.bothc 1.192092896E-15
.wlooc 9.5367432E-7
.numboxend
.keep break
.numboxbeg DBL_EPSILON
.ansi  <= 1E-9
.bothc 2.2204460492503131E-16
.wlooc 2.2204460492503129E-16
.numboxend
.keep break
.numboxbeg LDBL_EPSILON
.ansi  <= 1E-9
.bothc 2.2204460492503131E-16
.wlooc 2.2204460492503129E-16
.numboxend
.keep break
.bull
the minimum positive normalized floating-point number
.numboxbeg FLT_MIN
.ansi  <= 1E-37
.bothc 1.175494351E-38
.wlooc 5.3976053E-79
.numboxend
.keep break
.numboxbeg DBL_MIN
.ansi  <= 1E-37
.bothc 2.22507385850720160E-308
.wlooc 5.3976053469340275E-79
.numboxend
.keep break
.numboxbeg LDBL_MIN
.ansi  <= 1E-37
.bothc 2.22507385850720160E-308
.wlooc 5.3976053469340275E-79
.numboxend
.keep end
.endbull
.pp
As discussed in the section "&inttoflt.",
the macro
.mono FLT_ROUNDS
is replaced by a constant expression whose value indicates what
kind of rounding occurs following a floating-point operation.
The following table gives the value of
.mono FLT_ROUNDS
and its meaning:
..sk 1 c
.im fltround
.pc
If
.mono FLT_ROUNDS
has any other value, the rounding mechanism is
implementation-defined.
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.shade begin
For the &wcboth. compiler, the value of
.mono FLT_ROUNDS
is 1,
meaning that floating-point values are rounded to the nearest
representable number.
.shade end
..do end
..if '&target' eq 'PC 370' ..th ..do begin
.shade begin
For the &wlooc. compiler, the value of
.mono FLT_ROUNDS
is 0,
meaning that floating-point values are rounded towards zero.
.shade end
..do end
