.pp
Whenever two operands are involved in an operation,
some kind of
.ix 'conversion' 'type'
.ix 'type' 'conversion'
.us conversion
of one or both of the operands may take place.
For example, a
.kw short int
and a
.kw long int
cannot be directly added. Instead, the
.kw short int
must first be converted to a
.kw long int
..ct ,
then the two values can be added.
.pp
Fortunately, C provides most conversions as
.us implicit
operations. Simply by indicating that the two values
are to be added, the C compiler will check their types and
generate the appropriate conversions.
Sometimes it is necessary, however, to be aware of exactly
how C will convert the operands.
.pp
Conversion of operands always attempts to preserve
the value of the operand.
Where preservation of the value is not possible, the compiler will
sign-extend signed quantities and discard the high bits
of quantities being converted to smaller types.
.pp
The rules of type conversions are fully discussed in
the following sections.
.*
.section Integral Promotion
.ix 'integral promotion'
.ix 'promotion' 'integer'
.*
.begnote
.note Rule:
A
.kw char
..ct ,
.kw short int
or
.kw int
bit-field in either of their signed or unsigned forms, or an object
that has an enumerated type, is always converted to an
.kw int
..ct ..li .
If the type
.kw int
cannot contain the entire range of the object being converted, then
the object will be converted to an
.kw unsigned int
..ct ..li .
.endnote
.pp
A
.kw signed
or
.kw unsigned char
will be converted to a
.kw signed int
without changing the value.
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.shade begin
With &wc286., a
.kw short int
has the same range as
.kw int
..ct ,
therefore a
.kw signed short int
is converted to a
.kw signed int
..ct ,
and an
.kw unsigned short int
is converted to an
.kw unsigned int
..ct ,
without changing the value.
.pp
With &wc386., a
.kw signed
or
.kw unsigned
.kw short int
is converted to an
.kw int
without changing the value.
.shade end
..do end
..if '&target' eq 'PC 370' ..th ..do begin
.shade begin
With &wlooc., a
.kw signed
or
.kw unsigned
.kw short int
is converted to an
.kw int
without changing the value.
.shade end
..do end
.*
.************************************************************************
.*
.pp
These promotions are called the
.ix 'integral promotion'
.ix 'promotion' 'integer'
.us integral promotions.
.*
.section Signed and Unsigned Integer Conversion
.ix 'conversion' 'signed integer'
.ix 'conversion' 'unsigned integer'
.ix 'integer' 'conversion'
.*
.begnote
.note Rule:
.ix 'conversion' 'unsigned integer'
.ix 'unsigned integer conversion'
If an unsigned integer is converted to an integer type of any size,
then, if the value can be represented in the new type, the value
remains unchanged.
.endnote
.pp
If an unsigned integer is converted to a longer type (type with greater
range), then the value will not change.
If it is converted to a type with a smaller range, then provided
the value can be represented in the smaller range, the value will
remain unchanged. If the value cannot be represented, then if
the result type is signed, the result is implementation-defined.
If the result type is
.kw unsigned
..ct ,
the result is the integer modulo (1+the largest unsigned number that
can be stored in the shorter type).
.shade begin
With
..if '&target' eq 'PC' ..th ..do begin
&wc286.,
..do end
..el ..if '&target' eq 'PC 370' ..th .do begin
both &wcboth. and &wlooc.,
..do end
..el ..if '&target' eq '370' ..th ..do begin
&wlooc.,
..do end
unsigned integers are promoted to longer types by extending the
high-order bits with zeros.
They are demoted to shorter types by
discarding the high-order portion of the larger type.
.shade end
.pp
Consider the following examples of 32-bit quantities (
..ct .kw unsigned long int
..ct )
being converted
to 16-bit quantities (
..ct .kw signed short int
or
.kw unsigned short int
..ct ):
..sk 1 c
.if &e'&nobox eq 0 .do begin
.boxdef
..if '&format' eq '7x9' ..th ..do begin
.  .boxcol  6
.  .boxcol 10
.  .boxcol  9
.  .boxcol  6
.  .boxcol  8
..do end
..el ..do begin
.  .boxcol  7
.  .boxcol 13
.  .boxcol 11
.  .boxcol  7
.  .boxcol  9
..do end
.boxbeg
.monoon
$    $:HP0.32-bit:eHP0.        $:HP0.16-bit:eHP0.        $signed$unsigned
$long$:HP0.representation:eHP0.$:HP0.representation:eHP0.$short $short
.monooff
.boxline
.monoon
$65538 $0x00010002$0x0002$2     $2
$100000$0x000186A0$0x86A0$-31072$34464
.monooff
.boxend
.do end
.el .do begin
.millust begin
        32-bit           16-bit           signed   unsigned
long    representation   representation   short    short
------  --------------   --------------   ------   --------
65538   0x00010002       0x0002           2        2
100000  0x000186A0       0x86A0           -31072   34464
.millust end
.do end
.begnote
.note Rule:
When a signed integer is converted to an unsigned integer of
equal or greater length, if the value is non-negative, the
value will be unchanged.
.endnote
.pp
A non-negative value stored in a signed integer may be converted to
an equal or larger integer type without affecting the value.
A negative value is first converted
to the signed type of the same length as the result, then
(1+the largest unsigned number that can be stored in the result type)
is added to the value to convert it to the unsigned type.
.shade begin
With
..if '&target' eq 'PC' ..th ..do begin
&wc286.,
..do end
..el ..if '&target' eq 'PC 370' ..th .do begin
&wcboth. and &wlooc.,
..do end
..el ..if '&target' eq '370' ..th ..do begin
&wlooc.,
..do end
signed integers are promoted to longer types by
.ix 'sign extension'
.us sign-extending
the value (the high bit of the shorter type is propogated throughout
the high bits of the longer type).
When the longer type is unsigned, the sign-extended bit-pattern
is then treated as an unsigned value.
.shade end
.pp
Consider the following examples of 16-bit signed quantities (
..ct .kw signed short int
..ct ) being
converted to 32-bit quantities (
..ct .kw signed long int
and
.kw unsigned long int
..ct ):
..sk 1 c
.if &e'&nobox eq 0 .do begin
.boxdef
..if '&format' eq '7x9' ..th ..do begin
.  .boxcol  7
.  .boxcol  8
.  .boxcol 10
.  .boxcol  7
.  .boxcol 10
..do end
..el ..do begin
.  .boxcol  7
.  .boxcol 10
.  .boxcol 12
.  .boxcol  8
.  .boxcol 12
..do end
.boxbeg
.monoon
$signed$:HP0.16-bit$32-bit:eHP0.$signed$unsigned
$short $:HP0.represention$representation:eHP0.$long  $long
.monooff
.boxline
.monoon
$-2    $0xFFFE $0xFFFFFFFE$-2   $4294967294
$32766 $0x7FFE $0x00007FFE$32766$32766
.monooff
.boxend
.do end
.el .do begin
.millust begin
signed  16-bit         32-bit           signed   unsigned
short   represention   representation   long     long
------  ------------   --------------   ------   ----------
-2      0xFFFE         0xFFFFFFFE       -2       4294967294
32766   0x7FFE         0x00007FFE       32766    32766
.millust end
.do end
.begnote
.note Rule:
When a signed integer is converted to a longer signed integer,
the value will not change.
.note Rule:
When a signed integer is converted to a shorter type, the
result is implementation-defined.
.endnote
.pp
.shade begin
With
..if '&target' eq 'PC' ..th ..do begin
&wc286.,
..do end
..el ..if '&target' eq 'PC 370' ..th .do begin
&wcall.,
..do end
..el ..if '&target' eq '370' ..th ..do begin
&wlooc.,
..do end
signed integers are converted to a shorter type
by preserving the low-order (least significant)
portion of the larger type.
.shade end
.*
.section Floating-Point to Integer Conversion
.ix 'conversion' 'float to integer'
.ix 'float' 'conversion to integer'
.*
.begnote
.note Rule:
When a floating-point type is converted to integer, the fractional
part is discarded. If the value of the integer part cannot
be represented in the integer type, then the result is
undefined.
.endnote
.pp
Hence, it is valid only to convert a floating-point type to
integer within the range of the integer type being converted to.
Refer to
the section "Integer Types" for details on the range of integers.
.*
.section &inttoflt.
.ix 'integer' 'conversion to float'
.ix 'conversion' 'integer to float'
.*
.begnote
.note Rule:
When the value of an integer type is converted to a floating-point type,
and the integer value cannot be represented exactly in the floating-point
type, the value will be
rounded either up or down.
.endnote
.pp
.ix 'rounding'
.ix 'float' 'rounding'
Rounding of floating-point numbers is implementation-defined.
The technique being used by the compiler may be determined from the
macro
.mkw FLT_ROUNDS
found in the header
.hdr <float.h>
..ct ..li .
The following table describes the meaning of the various values:
..sk 1 c
.im fltround
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.shade begin
The &wcboth. compilers will round to the nearest number.
(The value of
.kwconst FLT_ROUNDS
is 1.)
.shade end
..do end
..if '&target' eq 'PC 370' ..th ..do begin
.shade begin
The &wlooc. compiler will round towards zero.
(The value of
.kwconst FLT_ROUNDS
is 0.)
.shade end
..do end
.*
.************************************************************************
.*
.begnote
.note Rule:
When a floating-point value is converted to a larger floating-point
type (
..ct .kw float
to
.kw double
..ct ,
.kw float
to
.kw long double
..ct , or
.kw double
to
.kw long double
..ct ), the value remains unchanged.
.note Rule:
When any floating-point type is demoted to a
floating-point type with a smaller range,
then the result will be undefined if the value lies outside the
range of the smaller type. If the value lies inside the range, but
cannot be represented exactly, then rounding will occur in an
implementation-defined manner.
.endnote
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.shade begin
The &wcboth. compilers round to the nearest number.
(The value of
.kwconst FLT_ROUNDS
is 1.)
.shade end
..do end
..if '&target' eq 'PC 370' ..th ..do begin
.shade begin
The &wlooc. compiler rounds towards zero.
(The value of
.kwconst FLT_ROUNDS
is 0.)
.shade end
..do end
.*
.************************************************************************
.*
.section Arithmetic Conversion
.*
.ix 'usual arithmetic conversion'
.ix 'arithmetic conversion'
.pp
Whenever two values are used with a binary operator that expects
arithmetic types (integer or floating-point), conversions may
take place implicitly. Most binary operators work on two values of the
same type. If the two values have different types, then the type
with the smaller range is always promoted to the type with the
greater range. Conceptually, each type is found in the table below
and the type found lower in the table is converted to the type
found higher in the table.
.millust begin
long double
double
float
unsigned long
long
unsigned int
int
.millust end
.pc
Note that any types smaller than
.kw int
have
.us integral promotions
performed on them to promote them to
.kw int
..ct ..li .
.keep begin
.pp
The following table illustrates the result type of performing
an addition on combinations of various types:
..sk 1 c
.if &e'&nobox eq 0 .do begin
.boxdef
..if '&format' eq '7x9' ..th ..do begin
.  .boxcol 23
.  .boxcol 12
..do end
..el ..do begin
.  .boxcol 28
.  .boxcol 14
..do end
.boxbeg
$ Operation                     $ Result Type
.boxline
.monoon
$signed char + signed char  $signed int
$unsigned char + signed int $signed int
$signed int + signed int    $signed int
$signed int + unsigned int  $unsigned int
$unsigned int + signed long $signed long
$signed int + unsigned long $unsigned long
$signed char + float        $float
$signed long + double       $double
$float + double             $double
$float + long double        $long double
.monooff
.boxend
.do end
.el .do begin
.millust begin
Operation                       Result Type
--------------------------      -------------
signed char + signed char       signed int
unsigned char + signed int      signed int
signed int + signed int         signed int
signed int + unsigned int       unsigned int
unsigned int + signed long      signed long
signed int + unsigned long      unsigned long
signed char + float             float
signed long + double            double
float + double                  double
float + long double             long double
.millust end
.do end
.keep end
.*
.************************************************************************
.*
.section Default Argument Promotion
.*
.ix 'default argument promotion'
.pp
When a call is made to a function, the C compiler checks to see if
the function has been defined already, or if a prototype for that
function has been found.
If so, then the arguments to the function are converted to the
specified types.
If neither is true, then the arguments to
the function are promoted as follows:
.begbull $compact
.bull
all integer types have the
.ix 'integral promotion'
.us integral promotions
performed on them, and,
.bull
all arguments of type
.kw float
are promoted to
.kw double
..ct ..li .
.endbull
.pp
If the definition of the function does not have parameters with types that
match the promoted types, the behavior is undefined.
