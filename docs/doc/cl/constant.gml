.ix 'constant'
.pp
A constant is a value which is fixed at compilation time and is
often just a number, character or string.
Every constant has a type
which is determined by its form and value.
For example, the value
.mono 1
may have the type
.kw signed int
..ct ,
while the value
.mono 400000
may have the type
.kw signed long
..ct ..li .
In many cases, the type of the constant does not matter.
If, for
example, the value
.mono 1
is assigned to an object of type
.kw long int
..ct ,
then the value
.mono 1
will be converted to a
long integer before the assignment takes place.
.*
.section Integer Constants
.*
.ix 'constant' 'integer'
.ix 'integer' 'constant'
.pp
An integer constant begins with a digit and contains no fractional
or exponent part. A prefix may be included which defines whether
the constant is in octal, decimal or hexadecimal format.
.pp
A constant
may be suffixed by
.mono u
or
.mono U
indicating an
.kw unsigned int
..ct , or by
.mono l
or
.mono L
indicating a
.kw long int
..ct , or by both indicating an
.kw unsigned long int
..ct ..li .
.pp
If a constant does not start with a zero and contains a sequence of
digits, then it is interpreted as a decimal (base 10) constant.
These are decimal constants:
.millust begin
7
762
98765L
.millust end
.pp
If the constant starts with
.mono 0x
or
.mono 0X
followed
by the digits from
.mono 0
through
.mono 9
and the letters
.mono a
(or
.mono A
..ct ) through
.mono f
(or
.mono F
..ct ), then the constant is interpreted as a hexadecimal (base 16)
constant.
The letters
.mono A
through
.mono F
represent the values
10 through 15 respectively.
These are hexadecimal constants:
.millust begin
0X07FFF
0x12345678L
0xFABE
.millust end
.keep begin
.pp
If a constant starts with a zero, then it is an octal constant
and may contain only the digits 0 through 7.
These are octal constants:
.millust begin
017
0735643L
0
.millust end
.keep end
.pc
Note that the constant
.mono 0
is actually
an octal constant, but is zero in decimal, octal and hexadecimal.
.pp
The following table describes what type the compiler will give to a
constant. The left column indicates what
base (decimal, octal or hexadecimal) is used
and what suffixes (
..ct .mono U
or
.mono L
..ct )
are present.
The right column indicates the types that may be given to such a
constant.
The type of an integer constant is the first type from the table
in which its value can be accurately represented.
..sk 1 c
.if &e'&nobox eq 0 .do begin
.boxdef
..if '&format' eq '7x9' ..th ..do begin
.boxcol 15
.boxcol 32
..do end
..el ..do begin
.boxcol 17
.boxcol 38
..do end
.* .cp 15
.boxbeg
$Constant              $Type
.boxline
.monoon
$:HP0.unsuffixed decimal:eHP0.    $int:HP0.,:eHP0. long:HP0.,:eHP0. unsigned long
.monooff
.boxline
.monoon
$:HP0.unsuffixed octal:eHP0.      $int:HP0.,:eHP0. unsigned int:HP0.,:eHP0. long:HP0.,:eHP0. unsigned long
.monooff
.boxline
.monoon
$:HP0.unsuffixed hexadecimal:eHP0.$int:HP0.,:eHP0. unsigned int:HP0.,:eHP0. long:HP0.,:eHP0. unsigned long
.monooff
.boxline
.monoon
$:HP0.suffix:eHP0. U :HP0.only:eHP0.         $unsigned int:HP0.,:eHP0. unsigned long
.monooff
.boxline
.monoon
$:HP0.suffix:eHP0. L :HP0.only:eHP0.         $long:HP0.,:eHP0. unsigned long
.monooff
.boxline
.monoon
$:HP0.suffixes:eHP0. U :HP0.and:eHP0. L$unsigned long
.monooff
.boxline
.monoon
$:HP0.suffix:eHP0. LL :HP0.only:eHP0.        $long long:HP0.,:eHP0. unsigned long long
.monooff
.boxline
.monoon
$:HP0.suffixes:eHP0. U :HP0.and:eHP0. LL$unsigned long long
.monooff
.boxend
.do end
.el .do begin
.millust begin
Constant                Type
----------------------- --------------------------------------
unsuffixed decimal      int, long, unsigned long

unsuffixed octal        int, unsigned int, long, unsigned long

unsuffixed hexadecimal  int, unsigned int, long, unsigned long

suffix U only           unsigned int, unsigned long

suffix L only           long, unsigned long

suffixes U and L        unsigned long

suffix LL only          long long, unsigned long long

suffixes U and LL       unsigned long long
.millust end
.do end
.keep begin
.pc
The following table illustrates a number of constants and their
interpretation and type:
.*
.************************************************************************
.*
..if '&target' eq 'PC' ..th ..do begin
..sk 1 c
.if &e'&nobox eq 0 .do begin
.boxdef
.if '&format' eq '7x9' .do begin
.boxcol 10
.boxcol 10
.boxcol 8
.boxcol 12
.boxcol 11
..do end
..el ..do begin
.boxcol 11
.boxcol 11
.boxcol 9
.boxcol 14
.boxcol 13
..do end
.* .cp 20
.boxbeg
$$$Hexa$
$$Decimal$  -decimal$&wc286.$&wc386.
$ Constant$ Value$ Value$ Type$ Type
.boxline
.smonoon
$33        $33        $21      $signed int   $signed int
$033       $27        $1B      $signed int   $signed int
$0x33      $51        $33      $signed int   $signed int
$33333     $33333     $8235    $signed long  $signed int
$033333    $14043     $36DB    $signed int   $signed int
$0xA000    $40960     $A000    $unsigned int $signed int
$0x33333   $209715    $33333   $signed long  $signed int
$0x80000000$2147483648$80000000$unsigned long$unsigned int
$2147483648$2147483648$80000000$unsigned long$unsigned int
$4294967295$4294967295$FFFFFFFF$unsigned long$unsigned int
.smonooff
.boxend
.do end
.el .do begin
.millust begin
                        Hexa-
              Decimal   decimal      &wc286.  &wc386.
 Constant      Value    Value         Type        Type
----------  ----------  --------  ------------  ------------
        33          33        21  signed int     signed int
       033          27        1B  signed int     signed int
      0x33          51        33  signed int     signed int
     33333       33333      8235  signed long    signed int
    033333       14043      36DB  signed int     signed int
    0xA000       40960      A000  unsigned int   signed int
   0x33333      209715     33333  signed long    signed int
0x80000000  2147483648  80000000  unsigned long  unsigned int
2147483648  2147483648  80000000  unsigned long  unsigned int
4294967295  4294967295  FFFFFFFF  unsigned long  unsigned int
.millust end
.do end
..do end
..if '&target' eq 'PC 370' ..th ..do begin
..sk 1 c
.if &e'&nobox eq 0 .do begin
.boxdef
.boxcol 11
.boxcol 11
.boxcol 9
.boxcol 14
.boxcol 13
.* .cp 20
.boxbeg
$        $       $        $          $&wc386.
$        $       $Hexa    $          $    and
$        $Decimal$-decimal$  &wc286. $ &wlooc.
$Constant$Value  $Value   $    Type  $    Type
.boxline
.monoon
$33        $33        $21      $signed int   $signed int
$033       $27        $1B      $signed int   $signed int
$0x33      $51        $33      $signed int   $signed int
$33333     $33333     $8235    $signed long  $signed int
$033333    $14043     $36DB    $signed int   $signed int
$0xA000    $40960     $A000    $unsigned int $signed int
$0x33333   $209715    $33333   $signed long  $signed int
$0x80000000$2147483648$80000000$unsigned long$unsigned int
$2147483648$2147483648$80000000$unsigned long$unsigned int
.monooff
.boxend
.do end
.el .do begin
.millust begin
                                                &wc386.
                        Hexa-                      and
              Decimal   decimal      &wc286.  &wlooc.
 Constant      Value    Value         Type        Type
----------  ----------  --------  ------------  ------------
        33          33        21  signed int     signed int
       033          27        1B  signed int     signed int
      0x33          51        33  signed int     signed int
     33333       33333      8235  signed long    signed int
    033333       14043      36DB  signed int     signed int
    0xA000       40960      A000  unsigned int   signed int
   0x33333      209715     33333  signed long    signed int
0x80000000  2147483648  80000000  unsigned long  unsigned int
2147483648  2147483648  80000000  unsigned long  unsigned int
.millust end
.do end
..do end
.*
.************************************************************************
.*
.keep end
.*
.section Floating-Point Constants
.*
.ix 'constant' 'floating-point'
.ix 'floating-point' 'constant'
.pp
A floating-point constant may be distinguished by the presence of either
a period, an
.mono e
or
.mono E
..ct , or both.
It consists of a value part
(mantissa) optionally followed by an exponent.
The mantissa may
include a sequence of digits representing a whole number, followed
by a period, followed by a sequence of digits representing a fractional
part. The exponent must start with an
.mono e
or
.mono E
followed by
an optional sign (:MONO.+:eMONO. or :MONO.-:eMONO.),
and a digit sequence representing
(with the sign)
the power of 10 by which the mantissa should be multiplied.
Optionally, the suffix
.mono f
or
.mono F
may be added indicating the constant
has type
.kw float
..ct ,
or the suffix
.mono l
or
.mono L
indicating the constant has type
.kw long double
..ct ..li .
If no suffix is present then the constant has type
.kw double
..ct ..li .
.pp
In the mantissa, either the whole number part or the fractional part
must be present. If only the whole number part is present and no
period is included then the exponent part must be present.
.keep begin
.pp
The following table illustrates a number of floating-point constants
and their type:
..sk 1 c
.if &e'&nobox eq 0 .do begin
.boxdef
..if '&format' eq '7x9' ..th ..do begin
.  .boxcol 10
.  .boxcol 12
.  .boxcol 11
..do end
..el ..do begin
.  .boxcol 11
.  .boxcol 13
.  .boxcol 12
..do end
.* .cp 12
.boxbeg
$ Constant $   Value    $ Type
.boxline
.monoon
$3.14159265$3.14159265E0$double
$11E24     $1.1E25      $double
$.5L       $5E-1        $long double
$7.234E-22F$7.234E-22   $float
$0.        $0E0         $double
.monooff
.boxend
.do end
.el .do begin
.millust begin
 Constant          Value          Type
----------      ------------    ----------
3.14159265      3.14159265E0    double
     11E24      1.1E25          double
       .5L      5E-1            long double
7.234E-22F      7.234E-22       float
        0.      0E0             double
.millust end
.do end
.keep end
.*
.section Character Constants
.*
.ix 'constant' 'character'
.ix 'character constant'
.*
.pp
A character constant is usually one character enclosed in single-quotes,
and indicates a constant whose value
is the representation of the character in the execution character set.
A character constant has type
.kw int
..ct ..li .
.pp
The character enclosed in quotes may be any character in the source
character set. Certain characters in the character set may not be
directly representable, since they may be assigned other
meanings. These characters can be entered using the following escape
sequences:
..sk 1 c
.if &e'&nobox eq 0 .do begin
.boxdef
..if '&format' eq '7x9' ..th ..do begin
.  .boxcol  8 c
.  .boxcol 12
.  .boxcol 14
..do end
..el ..do begin
.  .boxcol  8 c
.  .boxcol 14
.  .boxcol 16
..do end
.* .cp 15
.boxbeg
$Character$  Character Name $Escape Sequence
.boxline
.monoon
$'$:HP0.single quote:eHP0.     $\'
$"$:HP0.double quote:eHP0.     $" :HP0.or:eHP0. \"
$?$:HP0.question mark:eHP0.    $? :HP0.or:eHP0. \?
$\$:HP0.backslash:eHP0.        $\\
$ $:HP0.octal value:eHP0.      $\:ITAL.octal digits:eITAL.:HP0. (max 3):eHP0.
$ $:HP0.hexadecimal value:eHP0.$\x:ITAL.hexadecimal digits:eITAL.
.monooff
.boxend
.do end
.el .do begin
.millust begin
            Character           Escape
Character   Name                Sequence
---------   -----------------   --------------------
'           single quote        \'
"           double quote        " or \"
?           question mark       ? or \?
\           backslash           \\
            octal value         \octal digits (max 3)
            hexadecimal value   \xhexadecimal digits
.millust end
.do end
.keep begin
.pc
For example,
.millust begin
'a'     /* the letter a */
'\''    /* a single quote */
'?'     /* a question mark */
'\?'    /* a question mark */
'\\'    /* a backslash */
.millust end
.keep end
.pc
are all simple character constants.
.pp
The following are some character constants containing octal escape
sequences, made up of a
.mono \
followed by one, two or three octal digits
(the digits
.mono 0
through
.mono 7
..ct ):
.millust begin
'\0'
'\377'
'\100'
.millust end
.pc
If a character constant containing an octal value is found,
but a non-octal character is also present, or if a fourth octal digit is found,
it is not part of the octal character already specified, and constitutes
a separate character.
For example,
.millust begin
'\1000'
'\109'
.millust end
.pc
the first constant
is a two-character constant, consisting of the characters
.mono '\100'
and
.mono '0'
(because an octal value consists of at most three octal digits).
The second constant is also a two-character constant, consisting of the
characters
.mono '\10'
and
.mono '9'
(because
.mono 9
is not an octal digit).
.pp
If more than one octal value is to be specified in a character
constant, then each octal value must
be specified starting with
.mono \
..ct ..li .
.pp
The meaning of character constants with more than one character
is implementation-defined.
.pp
The following are some character constants containing hexadecimal escape
sequences, made up of a
.mono \x
followed by one or more hexadecimal digits
(the digits
.mono 0
through
.mono 9
..ct ,
and the letters
.mono a
through
.mono f
and
.mono A
through
.mono F
..ct ).
(The
values
of these character constants are the same as the first examples of
octal values
presented above.)
.millust begin
'\x0'
'\xFF'
'\x40'
.millust end
.pc
If a character constant containing a hexadecimal value is found,
but a non-hexadecimal character is also present,
it is not part of the hexadecimal character already specified, and constitutes
a separate character.
For example,
.millust begin
'\xFAx'
'\xFx'
.millust end
.pc
the first constant
is a two-character constant, consisting of the characters
.mono '\xFA'
and
.mono 'x'
(because
.mono x
is not a hexadecimal digit).
The second constant is also a two-character constant, consisting of the
characters
.mono '\xF'
and
.mono 'x'
..ct ..li .
.pp
If more hexadecimal digits are found than are required to specify
one character, the behavior is implementation-defined.
Specifically, any sequence of hexadecimal characters in a
hexadecimal value in a character constant is used to specify the
value of one character.
If more than one hexadecimal value is to be specified in a character
constant, then each hexadecimal value must
be specified starting with
.mono \x
..ct ..li .
.pp
The meaning of character constants with more than one character
is implementation-defined.
.pp
In addition to the above escape sequences,
the following escape sequences may be used to represent non-graphic
characters:
..im escape
.pp
The following trigraph sequences may be used to represent
characters not available on all terminals or systems:
.im trigraph
.*
.************************************************************************
.*
.shade begin
..if '&target' eq 'PC' ..th ..do begin
The &wcboth. compilers also allow
..do end
..el ..if '&target' eq 'PC 370' ..th ..do begin
The &wcall. compilers also allow
..do end
character constants with more than one
character.
These may be used to initialize larger types, such as
.kw int
..ct .li .
For example, the program fragment:
.millust begin
int code;
.*
code = 'ab';
.millust end
.pc
assigns the constant value
.mono 'ab'
to the integer object
.mono code
..ct ..li .
The letter
.mono b
is placed in the lowest order (least significant)
portion of the integer value and the letter
.mono a
is placed in the next highest portion.
.pp
Up to four characters may be placed in a character constant.
Successive
characters, starting from the right-most character in the constant,
are placed in successively higher order (more significant)
bytes of the result.
.* .pp
.* Multiple-character constants may not be portable to other
.* compilers, because they are not required by the ISO standard.
.shade end
.*
.************************************************************************
.*
.pp
Note that a character constant such as
.mono 'a'
is different from the corresponding string literal
.mono "a"
..ct ..li .
The former is of type
.kw int
and has the value of the letter
.mono a
in the execution character set.
The latter is of type "pointer to
.kw char
..ct "
and its value is the address of the first character
(
..ct .mono a
..ct )
of the
string literal.
.*
.beglevel
.section Wide Character Constants
.*
.ix 'wide character constant'
.ix 'character constant' 'wide'
.pp
If the value of a character constant is to be a
.ix 'multibyte character'
multibyte character from an extended character set, then a
.us wide character constant
should be specified.
Its form is similar to normal character constants,
except that the
constant is preceded by the character
.mono L
..ct ..li .
.pp
The type of a wide character constant is
.ix 'wchar_t'
.kw wchar_t
..ct ,
which is one of the integral types, and is described in the header
.hdr <stddef.h>
..ct ..li .
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.shade begin
With &wcboth.,
.ix 'wchar_t'
.kw wchar_t
is defined as
.kw unsigned short
..ct ..li .
.shade end
..do end
..if '&target' eq 'PC 370' ..th ..do begin
With &wlooc.,
.ix 'wchar_t'
.kw wchar_t
is defined as
.kw ??????????????
..ct ..li .
..do end
.*
.************************************************************************
.*
.pp
For example, the constant
.mono L'a'
is a wide character constant containing the letter
.mono a
from the source character set, and has type
.ix 'wchar_t'
.kw wchar_t
..ct ..li .
In contrast, the constant
.mono 'a'
is a character constant containing the letter
.mono a
..ct ,
and has type
.kw int
..ct ..li .
.pp
How the multibyte character maps onto the wide character value is
defined by the
.libfn mbtowc
library
function.
.pp
As shown above,
a wide character constant may also contain a single byte character, since
an extended character set contains the single byte characters.
The single byte character is mapped onto the corresponding
wide character code.
.endlevel
.*
.section String Literals
.*
.ix 'string literal'
.ix 'constant' 'string-literal'
.pp
A sequence of zero or more characters enclosed within double-quotes
is a
.ix 'string literal'
.us string literal.
.pp
Most of the same rules for creating character constants also apply
to creating string literals. However, the single-quote may be entered
directly or as the
.monoon
\'
.monooff
escape sequence. The double-quote must be entered as the
.monoon
\"
.monooff
escape sequence.
.pp
The value of a string
literal is the sequence of characters within the quotes, plus a
.ix 'null character'
null character at the end.
.pp
The type of a string literal is "array of
.kw char
..ct ".
.pp
The following are examples of string literals:
.millust begin
"Hello there"
"\"Quotes inside string\""
"G'day"
.millust end
.pp
If two or more string literals are adjacent, the compiler
will join them together into one string literal.
The pair of string literals,
.millust "Hello" "there"
would be joined by the compiler to be,
.millust "Hellothere"
and is an array of 11 characters, including the single terminating
null character.
.pp
The joining of adjacent string literals occurs
.us after
the replacement of escape sequences.
In the examples,
.millust begin
"\xFAB\xFA" "B"
"\012\01" "2"
.millust end
.pc
the first string, after joining, consists of three characters, with
the values
.mono '\xFAB'
..ct ,
.mono '\xFA'
and
.mono 'B'
..ct ..li .
The second string, after joining, also consists of three characters,
with the values
.mono '\012'
..ct ,
.mono '\01'
and
.mono '2'
..ct ..li .
.pp
A program should not attempt to modify a string literal, as
this behavior is undefined.
On computers
where memory can be protected, it is likely that string literals will
be placed where the program cannot modify them.
An attempt to
modify them will cause the program to fail.
On other computers without
such protection, the literal can be modified, but this is
generally considered to be
a poor programming practice. (Constants should be constant!)
.pp
A string literal normally is a string. It is not a string if one of the
characters within double-quotes is the null character
(
..ct .mono \0
..ct ).
If such a string literal is treated as a string, then only those
characters before the first null character will be considered part of
the string. The characters following the first null character
will be ignored.
.pp
If a source file uses the same string literal in several places, the compiler
may combine them so that only one instance of the string exists and each
reference refers to that string.
In other words, the addresses of each of the string literals would be the
same.
However,
no program should rely on this since other compilers may
make each string a separate instance.
.*
.************************************************************************
.*
.shade begin
The
..if '&target' eq 'PC' ..th ..do begin
&wcboth.
..do end
..el ..if '&target' eq 'PC 370' ..th ..do begin
&wcall.
..do end
compilers combine several instances of the same
string literal in the same module into a single string literal,
provided that they occur in declarations of constant objects
or in statements other than declarations (eg. assignment).
.shade end
.*
.************************************************************************
.*
.pp
If the program requires that several string literals be the same
instance, then an object should be declared as an array of
.kw char
with its value initialized to the string.
.beglevel
.*
.section Wide String Literals
.*
.ix 'string literal' 'wide'
.ix 'wide string literal'
.pp
If any of the characters in a string literal are
.ix 'multibyte character'
multibyte characters from an extended character set, then a
.us wide string literal
should be specified.
Its form is similar to normal string literals,
except that the
string is preceded by the character
.mono L
..ct ..li .
.pp
The type of a wide string literal is "array of
.ix 'wchar_t'
.kw wchar_t
..ct ".
.ix 'wchar_t'
.kw wchar_t
is one of the integral types, and is described in the header
.hdr <stddef.h>
..ct ..li .
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.shade begin
With &wcboth.,
.ix 'wchar_t'
.kw wchar_t
is defined as
.kw unsigned short
..ct ..li .
.shade end
..do end
..if '&target' eq 'PC 370' ..th ..do begin
With &wlooc.,
.ix 'wchar_t'
.kw wchar_t
is defined as
.kw ??????????????
..ct ..li .
..do end
.*
.************************************************************************
.*
.pp
For example, the string literal
.mono L"ab"
is a wide string literal containing the letters
.mono a
and
.mono b
..ct ..li .
Its type is "array [3] of
.ix 'wchar_t'
.kw wchar_t
..ct ", and the values of its elements are
.mono L'a'
..ct ,
.mono L'b'
and
.mono '\0'
..ct ..li .
In contrast, the string literal
.mono "ab"
has type "array [3] of
.kw char
..ct ", and the values of its elements are
.mono 'a'
..ct ,
.mono 'b'
and
.mono '\0'
..ct ..li .
.pp
How the multibyte characters map onto wide character values is
defined by the
.libfn mbtowc
library
function.
.pp
As shown above,
a wide string literal may also contain single byte characters, since
the extended character set contains the single byte characters.
The single byte characters are mapped onto the corresponding
wide character codes.
.pp
Adjacent wide string literals will be concatenated by the compiler
and a null character appended to the end.
If a string literal and a wide string literal are adjacent, the
behavior when the compiler attempts to concatentate them is
undefined.
.endlevel
