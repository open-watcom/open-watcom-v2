.ix 'implementation-defined behavior'
This appendix describes the behavior of &wlooc. when the standard
describes the behavior as
.us implementation-defined.
.keep begin
..sk 1 c
.bd The meaning of the arguments to the function
.us main.
.discuss begin
.pp
See the &userguide..
.discuss end
..sk 1 c
.keep break
.bd on
The number of significant characters in an object or function name
with external linkage.
.bd off
.discuss begin
.pp
The method of linking limits the number of
significant characters to 8.
.discuss end
..sk 1 c
.keep break
.bd on
The number of significant characters in an object or function name
without external linkage.
.bd off
.discuss begin
.pp
Unlimited.
.discuss end
.keep break
..sk 1 c
.bd on
Whether or not upper and lower case letters are distinct in an object
or function name with external linkage.
.bd off
.discuss begin
.pp
Upper and lower case letters are
.bd not
distinct.
.discuss end
.keep break
..sk 1 c
.bd on
The characters in the source and execution character sets.
.bd off
.discuss begin
.pp
The full EBCDIC character set is available in both the
.ix 'character set' 'source'
source and
.ix 'character set' 'execution'
execution character sets.
.discuss end
.keep break
..sk 1 c
.bd on
The mapping of characters from the source to the
execution character sets.
.bd off
.discuss begin
.pp
No mapping is done since the two character sets are the same.
.discuss end
.keep break
..sk 1 c
.bd on
The number of
chars in an int,
and their order.
.bd off
.discuss begin
.pp
There are four characters in an
.kw int
..ct ,
with the high order portion of the integer being stored at a lower
address than the low order portion.
There are two characters in a
.kw short int
..ct ..li .
.discuss end
.keep break
..sk 1 c
.bd on
The number of bits in a character in the execution character set.
.bd off
.discuss begin
.pp
8
.discuss end
.keep break
..sk 1 c
.bd on
The value of a character constant that contains a character or
escape sequence that is not in the execution character set.
.bd off
.discuss begin
.pp
Not possible.
.discuss end
.keep break
..sk 1 c
.bd on
The value of a character constant that contains more than one character.
.bd off
.discuss begin
.pp
Multiple-character constants are stored such that the right-most
character is stored in the lowest order (highest address) portion.
.discuss end
.keep break
..sk 1 c
.bd on
Whether the type
char
is treated as
signed
or
unsigned.
.bd off
.ix 'type' 'char'
.ix 'character type'
.discuss begin
.pp
.kw unsigned
..ct ..li .
.discuss end
.keep break
..sk 1 c
.bd on
The representations and ranges of the various kinds of integers.
.bd off
.discuss begin
.pp
Integers are stored using 2's complement form.
The high bit of each
signed integer is a sign bit.
If the sign bit is 1, the value is
negative.
.pp
The ranges of the various integer types are described in the section
"Integer Types".
.discuss end
.keep break
..sk 1 c
.bd on
The result of converting an integer to a shorter signed integer
when the value cannot be accurately represented.
.bd off
.discuss begin
.pp
The high order bits of the longer value are discarded, and the remaining
bits are interpreted according to the new type.
.discuss end
.keep break
..sk 1 c
.bd on
The result of converting an unsigned integer to a signed integer
of the same type when the value cannot be accurately represented.
.bd off
.discuss begin
.pp
The bit pattern is simply re-interpreted according to the new type.
.discuss end
.keep break
..sk 1 c
.bd on
The results of bitwise operations on signed integers.
.bd off
.discuss begin
.pp
The sign bit is treated as any other bit during bitwise operations.
At the completion of the operation, the new bit pattern is
interpreted according to the result type.
.discuss end
.keep break
..sk 1 c
.bd on
The sign of the remainder of a division operation.
.bd off
.discuss begin
.pp
The remainder has the same sign as the numerator.
.discuss end
.keep break
..sk 1 c
.bd on
The result of a right shift of a negative-valued signed integer.
.bd off
.discuss begin
.pp
A right shift of a signed integer will leave the higher, vacated bits
with the original value of the high bit.
In other words, the sign bit
is propogated to fill bits vacated by the shift.
.discuss end
.keep break
..sk 1 c
.bd on
The representations and ranges of the various floating-point types.
.bd off
.discuss begin
.pp
These are discussed in the section "Floating-Point Types".
Also, see "IBM System/370 Principles of Operation".
.discuss end
.keep break
..sk 1 c
.bd on
The truncation and/or rounding that occurs when an integer value is
converted to a floating-point type that cannot accurately represent
the value.
.bd off
.discuss begin
.pp
The value is truncated (rounded towards zero).
.discuss end
.keep break
..sk 1 c
.bd on
The truncation and/or rounding that occurs when a floating-point value
is converted to a shorter floating-point type.
.bd off
.discuss begin
.pp
The value is rounded towards zero.
.discuss end
.keep break
..sk 1 c
.bd on
The type of
.ix 'size_t'
.kw size_t
..ct ,
the result of the
sizeof
operator.
.bd off
.discuss begin
.pp
.kw unsigned int
..ct ..li .
.discuss end
.keep break
..sk 1 c
.bd on
The result of casting an integer to a pointer, or a pointer to
an integer.
.bd off
.discuss begin
.pp
For the sake of conversion, the pointer type is treated as
.kw unsigned int
..ct ..li .
The usual arithmetic conversions then take place.
.discuss end
.keep break
..sk 1 c
.bd on
The type of
.ix 'ptrdiff_t'
.kw ptrdiff_t
..ct ,
the result of subtracting two pointers.
.bd off
.discuss begin
.pp
.kw int
..ct ..li .
.discuss end
.keep break
..sk 1 c
.bd on
The number and type of objects with storage class
register
that can actually be placed in a machine register.
.bd off
.discuss begin
.pp
The &wlooc. compiler may place
.* .kw int
.* (both
.* .kw signed
.* and
.* .kw unsigned
.* ..ct ) values in registers.
any integer, floating-point or pointer values in registers.
.pp
The compiler will decide which objects will be placed in registers.
.discuss end
.keep break
..sk 1 c
.bd on
The result of accessing a union member using a member of a different
type than the one used to assign the value.
.bd off
.discuss begin
.pp
Undefined.
.discuss end
.keep break
..sk 1 c
.bd on
The padding and alignment of structure members.
.bd off
.discuss begin
.pp
The &wlooc. compiler does not align structure members by default.
A command line switch
may be used to force the compiler to do alignment.
See the &userguide. for details.
.discuss end
.keep break
..sk 1 c
.bd on
Whether an
int
.ix 'bit-field'
bit-field
is treated as
signed
or
unsigned.
.bd off
.discuss begin
.pp
.kw signed
..ct ..li .
.discuss end
.keep break
..sk 1 c
.bd on
Whether a bit-field that doesn't fit into the same
int
as another bit-field is placed in the next
int, or straddles the boundary.
.bd off
.discuss begin
.pp
Placed in the next
.kw int
..ct ..li .
.discuss end
.keep break
..sk 1 c
.bd on
The order that bit-fields are placed in an
int.
.bd off
.discuss begin
.pp
High order to low order.
.discuss end
.keep break
..sk 1 c
.bd on
The type allocated for enumerated types.
.bd off
.discuss begin
.pp
.kw int
..ct ..li .
.discuss end
.keep break
..sk 1 c
.bd on
The maximum number of declarators that may modify an arithmetic,
structure or union type.
.bd off
.discuss begin
.pp
Unlimited.
.discuss end
.keep break
..sk 1 c
.bd on
The maximum number of
.ix 'case label'
case
labels or values in a
.ix 'switch statement'
.ix 'statement' 'switch'
switch
statement.
.bd off
.discuss begin
.pp
Unlimited.
.discuss end
.keep break
..sk 1 c
.bd on
In a
.ix 'preprocessor directive' '#if'
.kwpp #if
or
.ix 'preprocessor directive' '#elif'
.kwpp #elif
preprocessor directive, whether a character constant matches the
same character in the execution character set, and whether a character
constant may be negative.
.bd off
.discuss begin
.pp
The character sets are the same so characters will match.
No character will be negative.
.discuss end
.keep break
..sk 1 c
.bd on
The method for finding
.ix 'header file'
header files.
.bd off
.discuss begin
.pp
See the &userguide..
.discuss end
.keep break
..sk 1 c
.bd on
The behavior of each recognized
.ix 'preprocessor directive' '#pragma'
.kwpp #pragma
directive.
.bd off
.discuss begin
.pp
See the &userguide for details.
.discuss end
.keep break
..sk 1 c
.bd on
The definitions for
.mkwix &mkwDATE_sp.
.mkwfont &mkwDATE.
and
.mkwix &mkwTIME_sp.
.mkwfont &mkwTIME.
when the date and/or time of compilation are not available.
.bd off
.discuss begin
.pp
The date and time are always available.
.discuss end
.keep break
..sk 1 c
.bd on
.ix 'null pointer'
.ix 'pointer' 'null'
The value of the null pointer constant.
.bd off
.discuss begin
.pp
.mono 0
.discuss end
.keep end
