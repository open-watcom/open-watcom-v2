.section EQUIVALENCE Statement
.*
.mbox begin
      EQUIVALENCE (nlist) [,(nlist)] ...
.mbox end
.synote 7
.mnote nlist
is a list of at least two names, each name separated by a comma.
.endnote
.np
The names appearing in
.id nlist
can be variable names, array names, array element names, character
names, character substring names, and character array element
substring names.
Dummy arguments are not allowed in
.id nlist.
.np
The
.kw EQUIVALENCE
statement specifies that the storage occupied by the entities appearing
in
.id nlist
all start at the same place.
It in no way changes the characteristics of an object.
For example, if a variable is equivalenced to an array, the variable
does not inherit the properties of the array.
Similarly, if a variable of type integer is equivalenced to a variable
of type real, there is no implied type conversion.
.np
If an array element name appears in an
.kw EQUIVALENCE
statement, the number of subscript expressions must be the same as the
number of dimensions specified when the array was declared and each
subscript expression must be in the range specified.
.xt on
As an extension to FORTRAN 77, &product allows a single subscript
expression for a multi-dimensional array.
.xt off
An array name used by itself is equivalent to specifying the first
element of the array.
.np
If a character substring appears in an
.kw EQUIVALENCE
statement, the substring defined by the substring expression must be
properly contained in the character entity being substrung.
A character name used by itself is equivalent to specifying the first
character of the character variable.
.exam begin
      REAL A,B
      DIMENSION A(10),B(20)
      EQUIVALENCE (A,B(16))
.exam end
.pc
In the above example, the first 5 elements of
.id A
occupy the same storage as the last 5 elements of
.id B.
.exam begin
      DIMENSION A(10)
      EQUIVALENCE (C,A(2)),(D,A(4))
.exam end
.pc
In the above example,
.id C
is assigned the same storage unit as
.id A(2)
and
.id D
is assigned the same storage unit as
.id A(4).
.np
.cp 20
.xt begin
The following example illustrates a &product extension.
.exam begin
      REAL A(2,10),B(20),C(2,2,5)
      EQUIVALENCE (A(5),B(1)),(B(1),C(1))
.exam end
.pc
In the above example, a single subscript is specified for arrays
.id A
and
.id C.
The following table shows the mapping of a 2-dimensional array
onto a 1-dimensional array.
.millust begin
      A(1,1)  ==  A(1)
      A(2,1)  ==  A(2)
      A(1,2)  ==  A(3)
      A(2,2)  ==  A(4)
      A(1,3)  ==  A(5)
      A(2,3)  ==  A(6)
              .
              .
              .
.millust end
.pc
In the above table, "==" is read as "is equivalent to".
.ix 'column major'
In FORTRAN, arrays are stored in "column major" format (i.e., arrays
are stored column by column rather than row by row).
.xt end
.exam begin
      CHARACTER*5 A, D
      EQUIVALENCE (A(3:5), D(1:3))
.exam end
.pc
In this example, the last 3 characters of
.id A
occupy the same character storage units as the first 3 characters of
.id D.
.np
There are certain restrictions on
.kw EQUIVALENCE
statements.
It is not possible to equivalence a storage unit to 2 different
storage units.
This is illustrated by the following example.
.exam begin
* Illegally equivalencing a storage unit to
* 2 different storage units.
      DIMENSION A(2)
      EQUIVALENCE (A(1),B),(A(2),B)
.exam end
.pc
.id B
has been given 2 different storage units.
.np
It is also not possible to specify that consecutive storage units be
non-consecutive.
For example,
.exam begin
* Illegally equivalencing consecutive storage units to
* non-consecutive storage units.
      DIMENSION A(2),B(2)
      EQUIVALENCE (A(1),B(2)),(A(2),B(1))
.exam end
.pc
.id A(1)
and
.id A(2)
are consecutive but
.id B(1)
and
.id B(2)
are not.
.np
.xt begin
The FORTRAN 77 standard specifies that character and numeric data cannot
be equivalenced;
&product allows character and numeric data to be equivalenced.
.xt end
