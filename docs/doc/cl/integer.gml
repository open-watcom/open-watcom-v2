.ix 'type' 'integer'
.pp
The most commonly used type is the integer. Integers are used for
storing most numbers that do not require a decimal point, such as
counters, sizes and
.ix 'array' 'index'
.ix 'index'
indices into arrays.
The range of integers is limited by the underlying machine architecture
and is usually determined by the range of values that can be handled by
the most convenient storage type of the hardware.
Most 16-bit machines can handle integers in the range
.mono -32768
to
.mono 32767
..ct ..li .
Larger machines
typically handle integers in the range
.mono -2147483648
to
.mono 2147483647
..ct ..li .
.pp
The general integer type includes a selection of types,
specifying whether
or not the value is to be considered as signed
(negative and positive values)
or unsigned (non-negative values), character (holds one character of
the character set), short
(small range), long (large range) or long long (very large range).
.pp
Just specifying the type
.kw int
indicates that the amount of storage should correspond to the most
convenient storage type of the hardware. The value is treated as being a
signed quantity.
According to the C language standard,
the minimum range for
.kw int
is
.mono -32767
to
.mono 32767
..ct ,
although a compiler may provide a greater range.
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.shade begin
With &wc286.,
.kw int
has a range of
.mono -32768
to
.mono 32767
..ct ..li .
.pp
With &wc386.,
.kw int
has a range of
.mono -2147483648
to
.mono 2147483647
..ct ..li .
.shade end
..do end
..if '&target' eq 'PC 370' ..th ..do begin
.shade begin
With &wlooc.,
.kw int
has a range of
.mono -2147483648
to
.mono 2147483647
..ct ..li .
.shade end
..do end
.*
.************************************************************************
.*
.pp
Specifying the type
.ix 'type' 'char'
.kw char
indicates that the amount of storage is large enough to store any member
of the
execution character set.
If a member of the required
source character set
(see "&charset.")
is stored in an object of type
.kw char
..ct ,
then the value is guaranteed to be positive.
Whether or not other
characters are positive is implementation-defined.
(In other words, whether
.kw char
is signed or unsigned is implementation-defined. If it is necessary for
the object of type
.kw char
to be signed or unsigned,
then the object should be declared explicitly,
as described below.)
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.shade begin
The &wcboth. compilers define
.kw char
to be
.kw unsigned
..ct ,
allowing objects of that type to store values in the range
.mono 0
to
.mono 255
..ct ..li .
A command line switch may be specified to cause
.kw char
to be treated as
.kw signed
..ct ..li .
This switch should only be used when porting a C program from a system
where
.kw char
is signed.
.shade end
..do end
..if '&target' eq 'PC 370' ..th ..do begin
.shade begin
The &wlooc. compiler for the IBM 370 defines
.kw char
to be
.kw unsigned
..ct ,
allowing objects of that type to store values in the range
.mono 0
to
.mono 255
..ct ..li .
.shade end
..do end
.*
.************************************************************************
.*
.pp
The
.kw int
keyword may be specified with the keywords
.ix 'type' 'short'
.kw short
or
.ix 'type' 'long'
.kw long
..ct ..li .
These keywords provide additional information about the range of values
to be stored in an object of this type.
According to the C language standard, a signed short integer has a
minimum range of
.mono -32767
to
.mono 32767
..ct ..li .
.* (as defined by
.* .mono SHRT_MIN
.* and
.* .mono SHRT_MAX
.* in
.* the header
.* .hdr <limits.h>
.* ..ct ).
A signed long integer has a minimum range of
.mono -2147483647
to
.mono 2147483647
..ct ..li .
.* (as defined by
.* .mono LONG_MIN
.* and
.* .mono LONG_MAX
.* in
.* .hdr <limits.h>
.* ..ct ).
.*
A signed long long integer has a minimum range of
.mono -9223372036854775807
to
.mono 9223372036854775807
..ct ..li .
.* (as defined by
.* .mono LLONG_MIN
.* and
.* .mono LLONG_MAX
.* in
.* .hdr <limits.h>
.* ..ct ).
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.shade begin
With &wcboth.,
.ix 'type' 'short'
.kw short int
has a range of
.mono -32768
to
.mono 32767
..ct ,
while
.ix 'type' 'long'
.kw long int
has a range of
.mono -2147483648
to
.mono 2147483647
..ct ,
and
.ix 'type' 'long long'
.kw long long int
has a range of
.mono -9223372036854775808
to
.mono 9223372036854775807
..ct ..li .
.shade end
..do end
..if '&target' eq 'PC 370' ..th ..do begin
.shade begin
With &wlooc.,
.ix 'type' 'short'
.kw short int
has a range of
.mono -32768
to
.mono 32767
..ct ,
while
.ix 'type' 'long'
.kw long int
has a range of
.mono -2147483648
to
.mono 2147483647
..ct ..li .
.shade end
..do end
.*
.************************************************************************
.*
.pp
The
.kw char
and
.kw int
types may be specified with the keywords
.ix 'type' 'short'
.kw signed
or
.ix 'type' 'long'
.kw unsigned
..ct ..li .
These keywords explicitly indicate whether the type represents
a signed or unsigned (non-negative) quantity.
.pp
The keyword
.kw int
may be omitted from the declaration if one (or more) of the keywords
.kw signed
..ct ,
.kw unsigned
..ct ,
.kw short
or
.kw long
is specified. In other words,
.kw short
is equivalent to
.kw signed short int
and
.kw unsigned long
is equivalent to
.kw unsigned long int
..ct ..li .
.pp
The appendix "&numlimit."
discusses a set of macro definitions
describing the range and other characteristics
of the various numeric types.
The macros from the header
.hdr <limits.h>
..ct ,
which describe the integer types,
are discussed.
.pp
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.cp 37
.shade begin
The following table describes all of the various integer types and
their ranges as implemented by the &wcboth. compilers.
Note that the table is in order of increasing
storage size.
..sk 1 c
.if &e'&nobox eq 0 .do begin
.boxdef
..if '&format' eq '7x9' ..th ..do begin
.  .boxcol 14
.  .boxcol 17
.  .boxcol 16
..do end
..el ..if '&format' eq '8.5x11' ..th ..do begin
.  .boxcol 16
.  .boxcol 19
.  .boxcol 19
..do end
..el ..do begin
.  .boxcol 18
.  .boxcol 21
.  .boxcol 20
..do end
.boxbeg
$                  $Minimum    $Maximum
$Type              $Value      $Value
.boxline
.smonoon
$signed char       $-128       $127
.smonooff
.boxline
.smonoon
$unsigned char     $0          $255
.smonooff
.boxline
.smonoon
$char              $0          $255
.smonooff
.boxline
.smonoon
$short int         $-32768     $32767
.smonooff
.boxline
.smonoon
$unsigned short int$0          $65535
.smonooff
.boxline
.smonoon
$int :HP0.(&c286.):eHP0.$-32768     $32767
.smonooff
.smonoon
$int :HP0.(&c386.):eHP0.$-2147483648$2147483647
.smonooff
.boxline
.smonoon
$unsigned int :HP0.(&c286.):eHP0.$0          $65535
.smonooff
.smonoon
$unsigned int :HP0.(&c386.):eHP0.$0          $4294967295
.smonooff
.boxline
.smonoon
$long int          $-2147483648$2147483647
.smonooff
.boxline
.smonoon
$unsigned long int $0          $18446744073709551615
.smonooff
.boxline
.smonoon
$long long int     $-92233720368547758078$9223372036854775807
.smonooff
.boxline
.smonoon
$unsigned long long$0     $18446744073709551615
.smonooff
.boxend
.do end
.el .do begin
.millust begin
                      Minimum               Maximum
Type                  Value                 Value
--------------------- --------------------- --------------------
signed char                            -128                  127

unsigned char                             0                  255

char                                      0                  255

short int                            -32768                32767

unsigned short int                        0                65535

int (&c286.)                           -32768                32767
int (&c386.)                      -2147483648           2147483647

unsigned int (&c286.)                       0                65535
unsigned int (&c386.)                       0           4294967295

long int                        -2147483648           2147483647

unsigned long int                         0           4294967295

long long int          -9223372036854775808  9223372036854775807

unsigned long long int                    0 18446744073709551615
.millust end
.do end
.* .us &wcboth. integer types and ranges
.pp
With &wc286., an object of type
.kw int
has the same range as an object of type
.kw short int
..ct ..li .
.pp
With &wc386., an object of type
.kw int
has the same range as an object of type
.kw long int
..ct ..li .
.shade end
..do end
..if '&target' eq 'PC 370' ..th ..do begin
.cp 37
.shade begin
The following table defines all of the various integer types and
their ranges as implemented by the &wlooc. compiler.
Note that the table is in order of increasing
storage size.
..sk 1 c
.boxdef
..if '&format' eq '7x9' ..th ..do begin
.  .boxcol 16
.  .boxcol 10
.  .boxcol 10
..do end
..el ..do begin
.  .boxcol 19
.  .boxcol 12
.  .boxcol 12
..do end
.boxbeg
$                  $Minimum    $Maximum
$Type              $Value      $Value
.boxline
.monoon
$signed char       $-128       $127
.monooff
.boxline
.monoon
$unsigned char     $0          $255
.monooff
.boxline
.monoon
$char              $0          $255
.monooff
.boxline
.monoon
$short int         $-32768     $32767
.monooff
.boxline
.monoon
$unsigned short int$0          $65535
.monooff
.boxline
.monoon
$int               $-2147483648$2147483647
.monooff
.boxline
.monoon
$unsigned int      $0          $4294967295
.monooff
.boxline
.monoon
$long int          $-2147483648$2147483647
.monooff
.boxline
.monoon
$unsigned long int $0          $4294967295
.monooff
.boxend
.us &wlooc. integer types and ranges
.pp
With &wlooc.,
an object of type
.kw int
has the same range as an object of type
.kw long int
..ct ..li .
.* ..ct ,
.* but they are still considered to be different types.
.shade end
..do end
.*
.************************************************************************
.*
.keep begin
.pp
The following are some examples of declarations of objects with
integer type:
.millust begin
char               a;
unsigned char      b;
signed char        c;
short              d;
unsigned short int e;
int                f,g;
signed             h;
unsigned int       i;
long               j;
unsigned long      k
signed long        l;
unsigned long int  m;
signed long long   n;
long long          o;
unsigned long long p;
long long int      q;
.millust end
.keep end
