.pp
.ix 'implementation-defined behavior'
This appendix describes the behavior of &wcboth. when the standard
describes the behavior as
.us implementation-defined.
The term describing each behavior is taken directly from the ISO/ANSI
C Language standard.
The numbers in parentheses at the end of each term refers to the
section of the standard that discusses the behavior.
.*
.section Translation
.*
.bigterms
.bigterm
.ix 'diagnostic'
How a diagnostic is identified (5.1.1.3).
.bigdesc
.pp
A diagnostic message appears as:
.cillust begin
filename:HP0.(:eHP0.line-number:HP0.)::eHP0.
error-type:HP0.!:eHP0.
msg-number:HP0.::eHP0.
msg_text
.cillust end
.pp
where:
..if &e'&dohelp eq 1 ..in 0
:ZDL termhi=1 tsize='1.0i' break.
:ZDT.filename
:ZDD.
is the name of the source file where the error was detected.
If the error was found in a file included from the source
file specified on the compiler command line, then the name of
the included file will appear.
:ZDT.line-number
:ZDD.is the source line number in the named file where the
error was detected.
:ZDT.error-type
:ZDD.is either the word
.mono Error
for errors that prevent the compile from completing successfully
(no code will be generated), or
.mono Warning
for conditions detected by the compiler that may
not do what the programmer expected, but
are otherwise valid. Warnings will not prevent the compiler from
generating code. The issuance of warnings may be controlled by a
command-line switch. See the &userguide. for details.
:ZDT.msg-number
:ZDD.
is the letter
.mono E
(for errors) followed by a four digit error number,
or the letter
.mono W
(for warnings)
followed by a three digit warning number.
Each message has its own unique message number.
:ZDT msg-text
:ZDD.
is a descriptive message indicating the problem.
:ZeDL.
.pp
Example:
.code begin
test.c(35): Warning! W301: No prototype found for 'GetItem'
test.c(57): Error! E1009: Expecting '}' but found ','
.code end
.endbigterms
.*
.section Environment
.*
.bigterms
.bigterm
The semantics of the arguments to main (5.1.2.2.1).
.bigdesc
.pp
Each blank-separated token, except within quoted strings,
on the command line is made into a string that is an element of
.ix 'argv'
.ix 'parameter' 'to main' 'argv'
.mono argv.
Quoted strings are maintained as one element.
.pp
For example, for the command line,
.millust pgm 2+ 1 tokens "one token"
.ix 'argc'
.ix 'parameter' 'to main' 'argc'
.mono argc
would have the value
.mono 5
..ct ,
and the five elements of
.mono argv
would be,
.millust begin
pgm
2+
1
tokens
one token
.millust end
.bigterm
What constitutes an interactive device (5.1.2.3).
.bigdesc
For &wcboth., the keyboard and the video display are considered
interactive devices.
.endbigterms
.*
..if &e'&dohelp eq 0 ..do begin
.section Identifiers
..do end
..el .do begin
.section Behaviour Concerning Identifiers
..do end
.*
.bigterms
.bigterm
The number of significant initial characters (beyond 31) in an
identifier without external linkage (6.1.2).
.bigdesc
.pp
Unlimited.
.keep begin
.bigterm
The number of significant initial characters (beyond 6) in an
identifier with external linkage (6.1.2).
.bigdesc
.pp
The &wcboth. compilers do not impose a limit.
The &lnkname. limits significant characters to 40.
.keep end
.bigterm
Whether case distinctions are significant in an identifier
with external linkage (6.1.2).
.bigdesc
.pp
The &wcboth. compilers produce object names in mixed case.
The &lnkname. provides an option to respect or ignore case when
resolving linkages.
By default, the linker respects case. See the &linkref. for details.
.endbigterms
.*
.section Characters
.*
.bigterms
.bigterm
The members of the source and execution character sets,
except as explicitly specified in the standard (5.2.1).
.bigdesc
.pp
The full IBM PC character set is available in both the
.ix 'character set' 'source'
source and
.ix 'character set' 'execution'
execution character sets. The set of values between 0x20 and 0x7F
are the
.ix 'character set' 'ASCII'
ASCII character set.
.bigterm
The shift states used for the encoding of multibyte characters (5.2.1.2).
.bigdesc
There are no shift states in the support for multibyte characters.
.bigterm
The number of bits in a character in the execution character set (5.2.4.2.1).
.bigdesc
.pp
8
.bigterm
The mapping of members of the source character set (in character
constants and string literals) to members of the execution
character set (6.1.3.4).
.bigdesc
.pp
Both the source and execution character sets are the full IBM PC
character set for whichever code page is in effect.
In addition, the following table shows escape sequences available
in the source character set, and what they translate to in the
execution character set.
.ix 'escape sequences'
.ix 'non-graphic characters' 'escape sequences'
..sk 1 c
.if &e'&nobox eq 0 .do begin
.boxdef
..if '&format' eq '7x9' ..th ..do begin
.  .boxcol 7
.  .boxcol 5
.  .boxcol 17
..do end
..el ..do begin
.  .boxcol 8
.  .boxcol 6
.  .boxcol 20
..do end
.boxbeg
$Escape  $Hex
$Sequence$Value$Meaning
.boxline
.monoon
$\a$07
.monooff
..ct $Bell or alert
.monoon
$\b$08
.monooff
..ct $Backspace
.monoon
$\f$0C
.monooff
..ct $Form feed
.monoon
$\n$0A
.monooff
..ct $New-line
.monoon
$\r$0D
.monooff
..ct $Carriage return
.monoon
$\t$09
.monooff
..ct $Horizontal tab
.monoon
$\v$0B
.monooff
..ct $Vertical tab
.monoon
$\'$27
.monooff
..ct $Apostrophe or single quote
.monoon
$\"$22
.monooff
..ct $Double quote
.monoon
$\?$3F
.monooff
..ct $Question mark
.monoon
$\\$5C
.monooff
..ct $Backslash
.monoon
$\:HP1.ddd:eHP1.$
.monooff
..ct $Octal value
.monoon
$\x:HP1.ddd:eHP1.$
.monooff
..ct $Hexadecimal value
.boxend
.do end
.el .do begin
.millust begin
Escape      Hex
Sequence    Value   Meaning
--------    -----   -------
\a          07      Bell or alert
\b          08      Backspace
\f          0C      Form feed
\n          0A      New-line
\r          0D      Carriage return
\t          09      Horizontal tab
\v          0B      Vertical tab
\'          27      Apostrophe or single quote
\"          22      Double quote
\?          3F      Question mark
\\          5C      Backslash
\ddd                Octal value
\xddd               Hexadecimal value
.millust end
.do end
.bigterm
The value of an integer character constant that contains a character or
escape sequence that is not represented in the execution character set
or the extended character set for a wide character constant (6.1.3.4).
.bigdesc
.pp
Not possible.
Both the source and execution character sets are the IBM PC character
set.
Thus, all characters in the source character set map directly to the
execution character set.
.bigterm
The value of an integer character constant that contains more
than one character or a wide character constant that contains
more than one multibyte character (6.1.3.4).
.bigdesc
.pp
A multi-character constant is stored with the right-most
character in the lowest-order
(least significant)
byte, and subsequent characters (moving to the left) being placed in
higher-order (more significant) bytes.
Up to four characters may be placed in a character constant.
.bigterm
The current locale used to convert multibyte characters into
corresponding wide characters (codes) for a wide character constant
(6.1.3.4).
.bigdesc
The &wcboth. compilers currently support only the
.mono "C"
locale, using North American English, and translates
code page 437 to UNICODE.
.pp
To support multibyte characters, a command line switch can be
used to indicate which multibyte character set to use.
See the &userguide. for details.
.bigterm
Whether a plain
.kw char
has the same range of values as
.kw signed char
or
.kw unsigned char
(6.2.1.1).
.ix 'type' 'char'
.ix 'character type'
.bigdesc
.pp
&wcboth. treat
.kw char
as
.kw unsigned
..ct ,
although a compiler command line switch
can be used to make it
.kw signed
..ct ..li .
.endbigterms
.*
.section Integers
.*
.bigterms
.bigterm
The representations and sets of values of the various types
of integers (6.1.2.5).
.bigdesc
.pp
Integers are stored using 2's complement form. The high bit of each
signed integer is a sign bit. If the sign bit is 1, the value is
negative.
.pp
The ranges of the various integer types are described in the section
"Integer Types".
.bigterm
The result of converting an integer to a shorter signed integer,
or the result of converting an unsigned integer to a signed integer
of equal length,
if the value cannot be represented (6.2.1.2).
.bigdesc
.pp
When converting to a shorter type,
the high-order bits of the longer value are discarded, and the remaining
bits are interpreted according to the new type.
.pp
For example, converting the signed long integer
.mono -15584170
(hexadecimal
.mono 0xFF123456
..ct )
to a signed short integer yields the result
.mono 13398
(hexadecimal
.mono 0x3456
..ct ).
.pp
When converting an unsigned integer to a signed integer of equal length,
the bits are simply re-interpreted according to the new type.
.pp
For example, converting the unsigned short integer
.mono 65535
(hexadecimal
.mono 0xFFFF
..ct )
to a signed short integer yields the result
.mono -1
(hexadecimal
.mono 0xFFFF
..ct ).
.bigterm
The results of bitwise operations on signed integers (6.3).
.bigdesc
.pp
The sign bit is treated as any other bit during bitwise operations.
At the completion of the operation, the new bit pattern is
interpreted according to the result type.
:CMT. .bigterm
:CMT. The number of
:CMT. chars in an int,
:CMT. and their order.
:CMT. .bigdesc
:CMT. .pp
:CMT. There are two characters in an
:CMT. .kw int
:CMT. ..ct ,
:CMT. with the low-order portion of the integer being stored at a lower
:CMT. address than the high-order portion. There are four characters in a
:CMT. .kw long int
:CMT. ..ct ..li .
.bigterm
The sign of the remainder on integer division (6.3.5).
.bigdesc
.pp
The remainder has the same sign as the numerator (left operand).
.keep begin
.bigterm
The result of a right shift of a negative-valued signed integral type (6.3.7).
.bigdesc
.pp
A right shift of a signed integer will leave the higher, vacated bits
with the original value of the high bit. In other words, the sign bit
is propogated to fill bits vacated by the shift.
.keep end
.pp
For example,
the result of
.mono ((short) 0x0123) >> 4
would be
.mono 0x0012.
The result of
.mono ((short) 0xFEFE) >> 4
will be
.mono 0xFFEF.
.endbigterms
.*
.section Floating Point
.*
.bigterms
.bigterm
The representations and sets of values of the various
types of floating-point numbers (6.1.2.5).
.bigdesc
.pp
These are discussed in the section "Floating-Point Types".
The floating-point format used is the IEEE Standard for
Binary Floating-Point Arithmetic as defined in the ANSI/IEEE
Standard 754-1985.
.bigterm
The direction of truncation when an integral number is converted to
a floating-point number that cannot exactly represent
the original value (6.2.1.3).
.bigdesc
.pp
Truncation is only possible when converting a
.kw long int
(signed or unsigned) to
.kw float
..ct ..li .
The 24 most-significant bits (including sign bit) are used.
The 25th is examined,
and if it is 1, the value is rounded up by adding one to the 24-bit
value.
The remaining bits are ignored.
.bigterm
The direction of
truncation or rounding when a floating-point number
is converted to a narrower floating-point number (6.2.1.4).
.bigdesc
.pp
The value is rounded to the nearest value in the smaller type.
.endbigterms
.*
.section Arrays and Pointers
.*
.bigterms
.bigterm
The type of integer required to hold the maximum size of an array ~-
that is, the type of the
.ix 'operator' 'sizeof'
.kw sizeof
operator,
.ix 'size_t'
.kw size_t
(6.3.3.4, 7.1.1).
.bigdesc
.pp
.kw unsigned int
.keep begin
.bigterm
The result of casting an integer to a pointer or vice versa (6.3.4).
.bigdesc
.pp
&wc286. conversion of pointer to integer:
.if &e'&nobox eq 0 .do begin
.boxdef
..if '&format' eq '7x9' ..th ..do begin
.  .boxcol  9
.  .boxcol 17
.  .boxcol 17
..do end
..el ..do begin
.  .boxcol 10
.  .boxcol 20
.  .boxcol 20
..do end
.boxbeg
$Pointer$:MONO.short int:eMONO.
$Type   $:MONO.int                $long int:eMONO.
.boxline
$near$result is pointer value     $result is DS register in
$    $                            $high-order 2 bytes, pointer
$    $                            $value in low-order 2 bytes
.boxline
$far $segment is discarded, result$result is segment in high-
$huge$is pointer offset (low-order$order 2 bytes, offset in
$    $2 bytes of pointer)         $low-order 2 bytes
.boxend
.do end
.el .do begin
.millust begin
Pointer     short int
Type        int                         long int
----------- --------------------------- ---------------------------
near        result is pointer value     result is DS register in
                                        high-order 2 bytes, pointer
                                        value in low-order 2 bytes
----------- --------------------------- ---------------------------
far        segment is discarded, result result is segment in high-
huge       is pointer offset (low-order order 2 bytes, offset in
           2 bytes of pointer)          low-order 2 bytes
.millust end
.do end
.keep end
.keep begin
.pp
.cp 13
&wc286. conversion of integer to pointer:
.if &e'&nobox eq 0 .do begin
.boxdef
..if '&format' eq '7x9' ..th ..do begin
.  .boxcol  9
.  .boxcol 17
.  .boxcol 17
..do end
..el ..do begin
.  .boxcol 10
.  .boxcol 20
.  .boxcol 20
..do end
.boxbeg
$Integer  $             $far pointer
$Type     $near pointer $huge pointer
.boxline
.monoon
$short int$:HP0.result is integer value     $result segment is DS:eHP0.
$int      $:HP0.                            $register, offset is:eHP0.
$         $:HP0.                            $integer value:eHP0.
.monooff
.boxline
.monoon
$long int $:HP0.result is low-order 2 bytes $result segment is high-:eHP0.
$         $:HP0.of integer value            $order 2 bytes, offset is:eHP0.
$         $:HP0.                            $low-order 2 bytes:eHP0.
.monooff
.boxend
.do end
.el .do begin
.millust begin
Integer                                 far pointer
Type        near pointer                huge pointer
----------- --------------------------- ---------------------------
short int   result is integer value     result segment is DS
int                                     register, offset is
                                        integer value
----------- --------------------------- ---------------------------
long int    result is low-order 2       result segment is high-
            byte of integer value       order 2 bytes, offset is
                                        low-order 2 bytes
.millust end
.do end
.keep end
.keep begin
.pp
&wc386. conversion of pointer to integer:
.if &e'&nobox eq 0 .do begin
.boxdef
..if '&format' eq '7x9' ..th ..do begin
.  .boxcol  9
.  .boxcol 17
.  .boxcol 17
..do end
..el ..do begin
.  .boxcol 10
.  .boxcol 20
.  .boxcol 20
..do end
.boxbeg
$Pointer$            $:MONO.int:eMONO.
$Type   $:MONO.short $long int:eMONO.
.boxline
$near$result is low-order 2 bytes $result is pointer value
$    $of pointer value            $
.boxline
$far $segment is discarded, result$segment is discarded, result
$huge$is low-order 2 bytes of     $is pointer offset
$    $pointer value               $
.boxend
.do end
.el .do begin
.millust begin
Pointer                                 int
Type        short                       long int
----------- --------------------------- ---------------------------
near        result is low-order 2       result is pointer value
            byte of pointer value
----------- --------------------------- ---------------------------
far         segment is discarded,       segment is discarded,
huge        result is low-order 2       result is pointer offset
            bytes of pointer value
.millust end
.do end
.keep end
.keep begin
.pp
&wc386. conversion of integer to pointer:
.if &e'&nobox eq 0 .do begin
.boxdef
..if '&format' eq '7x9' ..th ..do begin
.  .boxcol  9
.  .boxcol 17
.  .boxcol 17
..do end
..el ..do begin
.  .boxcol 10
.  .boxcol 20
.  .boxcol 20
..do end
.boxbeg
$Integer  $                             $far pointer
$Type     $near pointer                 $huge pointer
.boxline
.monoon
$short int$:HP0.result is integer value,  $result segment is DS:eHP0.
$         $:HP0.with zeroes for high-order$register, offset is integer:eHP0.
$         $:HP0.2 bytes                   $value, with zeroes for:eHP0.
$         $:HP0.                          $high-order 2 bytes:eHP0.
.monooff
.boxline
.monoon
$int      $:HP0.result is integer value   $result segment is DS:eHP0.
$long int $:HP0.                          $register, offset is:eHP0.
$         $:HP0.                          $integer value:eHP0.
.monooff
.boxend
.do end
.el .do begin
.millust begin
Integer                                 far pointer
Type        near pointer                huge pointer
----------- --------------------------- ---------------------------
short int   result is integer value,    result segment is DS
            with zeroes for high-order  register, offset is integer
            2 bytes                     value, with zeroes for
                                        high-order 2 bytes
----------- --------------------------- ---------------------------
int         result is integer value     result segment is DS
long int                                register, offset is
                                        integer value
.millust end
.do end
.keep end
.bigterm
The type of integer required to hold the difference between two
pointers to elements of the same array,
.ix 'ptrdiff_t'
.kw ptrdiff_t
(6.3.6, 7.1.1).
.bigdesc
.pp
If the
.ix 'huge memory model'
.ix 'memory model' 'huge'
huge memory model is being used,
.ix 'ptrdiff_t'
.kw ptrdiff_t
has type
.kw long int
..ct ..li .
.pp
For all other memory models,
.ix 'ptrdiff_t'
.kw ptrdiff_t
has type
.kw int
..ct ..li .
.pp
If two huge pointers are subtracted and the huge memory model is
not being used,
then the result type will be
.kw long int
even though
.ix 'ptrdiff_t'
.kw ptrdiff_t
is
.kw int
..ct ..li .
.endbigterms
.*
.section Registers
.*
.bigterms
.bigterm
The extent to which objects can actually be placed in registers by
use of the
.kw register
storage-class specifier (6.5.1).
.bigdesc
.pp
The &wcboth. compilers may place any object that is sufficiently small,
including a small structure,
in one or more registers.
.pp
The number of objects that can be placed in registers varies, and is
decided by the compiler.
The keyword
.kw register
does not control the placement of objects in registers.
.endbigterms
.*
.keep begin
.*
.section Structures, Unions, Enumerations and Bit-Fields
.*
.bigterms
.bigterm
A member of a union object is accessed using a member of a different
type (6.3.2.3).
.bigdesc
.pp
The behavior is undefined.
Whatever bit values are present as were stored via one member will be
extracted via another.
.keep end
.bigterm
The padding and alignment of members of structures (6.5.2.1).
.bigdesc
.pp
The &wcboth. compilers align structure members by default.
A command line switch, or the
.mono pack
pragma,
may be used to override the default.
See the &userguide. for default values and other details.
.bigterm
Whether a "plain"
.kw int
.ix 'bit-field'
bit-field
is treated as a
.kw signed int
bit-field or as an
.kw unsigned int
bit-field (6.5.2.1).
.bigdesc
.pp
.kw signed int
.bigterm
The order of allocation of bit-fields within a unit (6.5.2.1).
.bigdesc
.pp
Low-order (least significant) bit to high-order bit.
.bigterm
Whether a bit-field can straddle a storage-unit boundary (6.5.2.1).
.bigdesc
.pp
Bit-fields may not straddle storage-unit boundaries.
If there is insufficient room to store a subsequent bit-field in a
storage-unit, then it will be placed in the next storage-unit.
.bigterm
The integer type chosen to represent the values of an
enumeration type (6.5.2.2).
.bigdesc
.pp
By default, &wcboth. will use the smallest integer type that
can accommodate all values in the enumeration. The first appropriate type
will be chosen according to the following table:
.keep begin
..sk 1 c
..im enumtab
.keep end
.pp
Both compilers have a command-line switch that force all enumerations
to type
.kw int
..ct ..li .
See the &userguide. for details.
.endbigterms
.*
.section Qualifiers
.*
.bigterms
.bigterm
What constitutes an access to an object that has volatile-qualified
type (6.5.5.3).
.bigdesc
Any reference to a volatile object is also an access to that object.
.endbigterms
.*
.section Declarators
.*
.bigterms
.bigterm
The maximum number of declarators that may modify an arithmetic,
structure or union type (6.5.4).
.bigdesc
.pp
Limited only by available memory.
.endbigterms
.*
..if &e'&dohelp eq 0 ..do begin
.section Statements
..do end
..el .do begin
.section Behaviour Concerning Statements
..do end
.*
.bigterms
.bigterm
The maximum number of
.ix 'case label'
.kw case
values in a
.ix 'switch statement'
.ix 'statement' 'switch'
.kw switch
statement (6.6.4.2).
.bigdesc
.pp
Limited only by available memory.
.endbigterms
.*
.section Preprocessing Directives
.*
.bigterms
.bigterm
Whether the value of a single-character character constant in a constant
expression that controls conditional inclusion matches the value of the
same character constant in the execution character set.
Whether such a character constant may have a negative value (6.8.1).
.bigdesc
.pp
The character sets are the same so characters will match.
Character constants are unsigned quantities, so
no character will be negative.
.bigterm
The method for locating includable source files (6.8.2).
.ix 'header'
.bigdesc
.pp
See the &userguide. for full details of how included files are located.
.bigterm
The support of quoted names for includable source files (6.8.2).
.bigdesc
See the &userguide. for full details of how included files are located.
.bigterm
The mapping of source file character sequences (6.8.2).
.bigdesc
The source and execution character sets are the same.
Escape sequences are not supported in preprocessor directives.
.bigterm
The behavior of each recognized
.kwpp #pragma
directive (6.8.6).
.bigdesc
.pp
See the &userguide..
.bigterm
The definitions for
.mkwix &mkwDATE_sp.
.mkwfont &mkwDATE.
and
.mkwix &mkwTIME_sp.
.mkwfont &mkwTIME.
when respectively, the date and time of translation are not available (6.8.8).
.bigdesc
.pp
The date and time are always available.
.endbigterms
.*
.section Library Functions
.*
.bigterms
.bigterm
.ix 'null pointer'
.ix 'pointer' 'null'
.ix 'NULL macro'
The null pointer constant to which the macro
.mkw NULL
expands (7.1.6).
.bigdesc
.pp
For &wc286., the
.mkw NULL
macro expands to
.mono 0
for the
.ix 'small memory model'
.ix 'memory model' 'small'
small and
.ix 'medium memory model'
.ix 'memory model' 'medium'
medium
.ix 'small data'
.ix 'memory model' 'small data'
(small data)
memory models,
and to
.mono 0L
for the
.ix 'compact memory model'
.ix 'memory model' 'compact'
compact,
.ix 'large memory model'
.ix 'memory model' 'large'
large and
.ix 'huge memory model'
.ix 'memory model' 'huge'
huge
.ix 'big data'
.ix 'memory model' 'big data'
(big data)
memory models.
.pp
For &wc386., the
.mkw NULL
macro expands to
.mono 0.
.endbigterms
.pp
The implementation-defined behavior of the library functions is
described in the &libref..
