.section DATA Statement
.*
.mbox begin
      DATA nlist/clist/ [[,]nlist/clist/] ...
.mbox end
.synote 7
.mnote nlist
is a list of variable names, array element names, substring names and
implied-DO lists.
.mnote clist
is a list of the form:
.np
.mono a [,a] ...
.mnote a
is one of the forms:
.np
.mono c
.np
.mono r*c
(equivalent to
.id r
successive appearances of
.id c)
.mnote c
is a constant or the symbolic name of a constant
.mnote r
is an unsigned positive integer constant or the symbolic name of a
constant.
.endnote
.np
The items of
.id nlist
are initialized with the values specified in
.id clist
in the following manner.
The first item in
.id nlist
is assigned the value of the first item in
.id clist,
the second item in
.id nlist
is assigned the value of the second item in
.id clist,
etc.
In this way all items of
.id nlist
are initialized.
.np
The number of items in
.id nlist
must equal the number of items in
.id clist
so that a one-to-one correspondence exists between the two lists.
If an array without a subscript list appears in
.id nlist
there must be an element in
.id clist
for each element of the array.
.np
If the type of an entity in
.id nlist
is character or logical then the type of its corresponding item in
.id clist
must also be character or logical respectively.
.xt on
As an extension to FORTRAN 77, &product permits an item of type
character to be initialized with integer data.
.xt off
.np
An item of type character is initialized using the rules of assignment.
If the length of the item in
.id nlist
is greater than the length of the corresponding character constant in
.id clist,
the rightmost remaining characters in the item are initialized with
blanks.
If the length of the item in
.id nlist
is less than the length of the character constant in
.id clist,
the character constant is truncated to the length of the item in
.id nlist.
Note that initializing a character entity causes all of the characters
in the entity to become defined and that each character constant defines
exactly one character variable, array element or substring.
.np
If the type of an entity in
.id nlist
is integer, real, double precision or complex then the corresponding
item in
.id clist
can be one of integer, real, double precision or complex.
If necessary the constant in
.id clist
is converted to the type of the item in
.id nlist
according to the rules of arithmetic conversion (see the chapter
entitled :HDREF refid='fassmnt'.).
.np
A variable, array element or substring can only be initialized once.
If two entities are associated (for example equivalenced), only one of
the items can be initialized.
.exam begin
      CHARACTER*30 MSG
      LOGICAL TRUE
      REAL X, Y(10)
      DATA X/1.0/, TRUE/.TRUE./, MSG/'ENTER DATA'/
      DATA Y/10*5/
.exam end
.np
An
.us implied-DO list
.ix 'implied-DO list'
in a
.kw DATA
statement has the following form:
.millust begin
( dlist, i = m1, m2[, m3] )
.millust end
.synote 11
.mnote dlist
is a list of array element names and implied-DO lists.
.mnote i
is the name of an integer variable called the implied-DO-variable.
.mnote m1,m2,m3
are each integer constant expressions.
The expressions may contain implied-DO-variables of other implied-DO
lists that have this implied-DO list in their ranges.
.endnote
.np
The range of the implied-DO list is the list
.id dlist.
An iteration count and the value of the implied-DO-variable are
computed from
.id m1,m2
and
.id m3
in the same way as for a DO-loop except that the iteration count must
be positive.
An implied-DO-variable does not affect the definition of a variable by
the same name in the same program unit.
An implied-DO list is processed as follows.
Each item in the implied-DO list is processed once for each iteration
with the appropriate substitution of values for any occurrence of the
implied-DO-variable.
The following example initializes the upper right triangle of the
array A.
.exam begin
      DIMENSION A(5,5)
      DATA ((A(I,J),J=1,I),I=1,5)/15*0/
.exam end
.np
Dummy arguments, functions, and entities in blank common are not allowed
in
.id nlist.
Entities in a named common block can be initialized only within a
.us block data subprogram.
.ix 'block data subprogram'
.ix 'subprogram' 'block data'
.np
The following extensions to data initialization are supported by &product..
.autonote
.keep
.note
.xt begin
Character constants can initialize a variable of any type.
If the item in
.id nlist
is of numeric type and is being initialized with character data, the
size of the item in
.id nlist
is the maximum number of characters that can be stored in the space
allocated for that item.
The rules for initializing such items, are the same as for items
of type character.
See the chapter entitled :HDREF refid='ftypes'. for the number of
bytes required for a particular data type.
.exam begin
      INTEGER I,J
      DATA I/'AA'/,J/'123456'/
.exam end
.pc
In the previous example,
.id I
and
.id J
each occupy 4 character storage units.
.id I
will be initialized with the characters
.mono AA
followed by 2 blank characters.
.id J
will be initialized with the characters
.mono 1234.
Note the the character constant initializing
.id J
is truncated on the right to the number of character storage units
occupied by
.id J.
.xt end
.keep
.note
.xt begin
As an extension to FORTRAN 77, &product permits an item of type
character to be initialized with integer data.
.exam begin
      CHARACTER C, D
      DATA C/65/, D/66/
      END
.exam end
.xt end
.keep
.note
.xt begin
&product allows data initialization using hollerith constants.
Initializing items using hollerith constants behaves in the same way
as initializing items using character constants.
Note that hollerith data can initialize entities of any type.
See the chapter entitled :HDREF refid='ftypes'. for a description of
hollerith constants.
.xt end
.keep
.note
.xt begin
&product allows data initialization using hexadecimal or octal
constants.
Hexadecimal or octal constants can be used to initialize memory with
any binary pattern.
.np
Items are initialized with hexadecimal constants in the following way.
Two hexadecimal digits are required to initialize one byte of storage.
If the number of characters in the hexadecimal constant is less than 2
times the number of bytes of storage allocated for the entity being
initialized, the entity is padded on the left with zeroes.
If the number of characters in the hexadecimal constant is greater
than 2 times the number of bytes of storage allocated for
the entity being initialized, the constant is truncated on the left to
the size (in bytes) of the entity being initialized.
.np
Items are initialized with octal constants in the following way.
Each octal digit initializes three bits of storage.
If the number of digits in the octal constant times 3 is less than the
number of bits of storage allocated for the entity being initialized,
the entity is padded on the left with zero bits.
If the number of digits in the octal constant times 3 is greater than
the number of bits of storage allocated for the entity being
initialized, bits are truncated on the left to the size (in bits) of
the entity being initialized.
.np
Note that hexadecimal or octal data can initialize entities of any
type.
See the chapter entitled :HDREF refid='ftypes'. for a description of
hexadecimal and octal constants.
.cp 17
.exam begin
      DOUBLE PRECISION DPREC
      COMPLEX CMPLX
* Initialize an integer variable with the value 5
      DATA I/Z05/
* Initialize a real variable with the value 5.0
      DATA X/Z41500000/
* Initialize a double precision variable
*   with the value 5D0
      DATA DPREC/Z4150000000000000/
* Initialize a complex variable
*   with the value (5.0,5.0)
      DATA CMPLX/Z4150000041500000/
          .
          .
          .
      END
.exam end
.xt end
.keep
.np
.xt begin
Caution should be used when initializing items with hexadecimal
constants, in particular those whose type is real or double precision,
since the data they represent depends on the computer being used.
In the previous example, the hexadecimal constant used to initialize the
variable
.id X,
represents the number 5.0 on a computer with an IBM 370 architecture.
The number 5.0 will have a different floating-point representation on
other computers.
.xt end
.endnote
