.section DIMENSION Statement
.*
.mbox begin
      DIMENSION a(d) [,a(d)] ...
.mbox end
.synote
.mnote a
is the name of the array.
.mnote d
defines the dimension of the array and the range of its subscripts.
See the chapter entitled :HDREF refid='farrays'. for more information
on dimensioning arrays.
.endnote
.np
Each name
.id a
appearing in a
.kw DIMENSION
statement defines
.id a
to be an array in the program unit containing the
.kw DIMENSION
statement.
A name can only be dimensioned once in a program unit.
Note that a name can also be dimensioned in a
.kw COMMON
statement and type declaration statements.
.exam begin
      DIMENSION A(10), B(-5:5), C(I,J), D(4,*)
.exam end
.pc
In this example
.id A
is a 1-dimensional array containing 10 elements, each element referenced
as
.id A(1),
.id A(2),
.id ...,
.id A(9),
.id A(10).
.id B
is a 1-dimensional array containing 11 elements, each element referenced
as
.id B(-5),
.id B(-4),
.id ...,
.id B(4),
.id B(5).
.id C
is a 2-dimensional array containing
.id I
rows and
.id J
columns.
.id C,
.id I,
and
.id J
must be
.us dummy arguments
.ix 'dummy argument'
.ix 'argument' 'dummy'
or belong to a common block.
.id D
is a 2-dimensional array containing 4 rows.
The
.mono *
in the last dimension indicates that
.id D
is an
.us assumed size array.
.ix 'array' 'assumed-size'
.ix 'assumed-size array'
.id D
must be a
.us dummy argument.
The number of columns is determined from the number of elements
of the
.us actual argument.
.ix 'argument' 'actual'
.ix 'actual argument'
For example, if the actual argument contains 8 elements then
.id D
would contain 2 columns (i.e., 8 elements / 4 rows).
.np
For more information on dimensioning arrays refer to the chapter
entitled :HDREF refid='farrays'..
See also the description of the
.kw ALLOCATE
and
.kw DEALLOCATE
statements for information on dynamically allocatable arrays.
