.chap *refid=farrays Arrays
.*
.if &e'&dohelp eq 0 .do begin
.section Introduction
.do end
.*
.np
An array is a non-empty collection of data.
Arrays allow a convenient way of manipulating large quantities of data.
An array can be referenced as an entity.
In this way it is possible to conveniently pass large quantities of
data between subprograms.
Alternatively, it is possible to reference each element of an array
individually so that data can be selectively processed.
Consider the task of managing the marks of 100 students.
Without arrays one would have to have a unique name for each mark.
They might be M1, M2, etc. up to M100.
This is clearly cumbersome.
Instead, we can use an array called MARKS containing 100 elements.
Now there is one name for all the marks.
Each mark can be referenced by using that name followed by a subscript.
Furthermore, suppose the size of the class doubled.
Do we add the names M101, M102, etc. up to M200?
Not if we use arrays.
If the size of the class doubled, all that need be done is to define
the array to contain 200 elements.
It is not hard to see that programs that use arrays tend to be
general in nature.
Arrays also facilitate the repetitive computations that must be performed
on large amounts of data in that they lend themselves to loop
processing.
.*
.section Properties of Arrays
.*
.np
Arrays are defined by an array declarator.
The form of an
.us array declarator
.ix 'array declarator'
is:
.mbox begin
      a( d [,d] ... )
.mbox end
.synote
.mnote a
is the symbolic name of the array
.mnote d
is a dimension declarator.
.endnote
.np
The number of dimensions of the array is determined by the number
of dimension declarators appearing in the array declarator.
Allowable dimensions for arrays range from 1 to 7.
A 1-dimensional array can be viewed as a vector, a 2-dimensional array
as a matrix and a 3-dimensional array as a number of parallel matrices.
Arrays with dimension higher than 3 are generally difficult to
intuitively describe and hence examples will deal with arrays whose
dimension is 1, 2 or 3.
.np
Each dimension has a range of values.
When referencing elements in that dimension, the dimension expression
must fall in that range.
The range of a dimension is defined in the dimension declarator.
A
.us dimension declarator
.ix 'dimension declarator'
has the following form:
.mbox begin
      [lo:] hi
.mbox end
.synote
.mnote lo
is the lower dimension bound.
.mnote hi
is the upper dimension bound.
.endnote
.np
The lower and upper dimension bounds must be integer expressions and
the upper dimension bound must be greater than or equal to the lower
dimension bound.
The upper dimension bound of the last dimension may be an asterisk
.mono (*).
The meaning of this will be discussed later.
If the lower dimension bound is not specified then a default of 1
is assumed.
The size of a dimension is defined as
.id hi &minus. lo + 1.
Note that if the lower dimension bound is not specified the size of the
dimension is just
.id hi.
The size of the array (or the number of elements in the array)
is defined as the product of all the sizes of the
dimensions of the array.
.ix 'maximum' 'number of array elements'
.ix 'array elements' 'maximum'
.ix 'array declarator' 'maximum number of elements'
The maximum number of elements in any dimension is limited to 65535.
.ix 'maximum' 'size of an array'
.ix 'array' 'maximum size'
The maximum size of an array is limited by the amount of available
memory.
.np
Arrays are defined by the appearance of an array declarator in a
.kw DIMENSION
statement,
a type statement or a
.kw COMMON
statement.
.exam begin
      DIMENSION A(10), B(-5:5,-10:10)
      INTEGER C(10,20)
      COMMON /DATA/ X,Y(30,30),Z
.exam end
.pc
In the previous example, B is a 2-dimensional array with 11 rows and
21 columns and has 231 elements (i.e. 11 * 21).
.np
Each array has a data type associated with it.
This data type is inherited by all elements of the array.
.*
.section Array Elements
.*
.np
Each array is comprised of a sequence of array elements.
An array element is referenced by following the array name with a
.us subscript.
.ix 'subscript'
Different elements of the array are referenced by simply
changing the subscript.
An
.us array element
.ix 'array element'
has the following form:
.mbox begin
      a(s[,s]...)
.mbox end
.synote 14
.mnote a
is the array name.
.mnote (s[,s]...)
is a subscript.
.mnote s
is a subscript expression.
.endnote
.np
Each
.us subscript expression
.ix 'subscript expression'
must be an integer expression and must be in the range defined by the
upper and lower dimension bounds of the corresponding dimension.
The number of subscript expressions must be equal to the dimension of
the array.
.np
If an array has
.us
n
elements then there is a 1-to-1 correspondence between the elements
of the array and the integers from 1 to
.us n.
Each subscript has a
.us subscript value
.ix 'subscript value'
associated with it which determines which element of the array is
being referenced.
If the subscript value is
.us i
then the
.us i
.ct th
element of the array is the one referenced.
The subscript value depends on the subscript expressions and on the
dimensions of the array.
The following table describes how to compute the subscript value.
:cmt. 1    (j&s'1.:k&s'1.)    (s&s'1.)    1+(s&s'1.&minus.j&s'1.)
:cmt. 2    (j&s'1.:k&s'1.,j&s'2.:k&s'2.)    (s&s'1.,s&s'2.)    1+(s&s'1.&minus.j&s'1.)
:cmt.                                                           +(s&s'2.&minus.j&s'2.)*d&s'1.
:cmt. 3    (j&s'1.:k&s'1.,j&s'2.:k&s'2.,j&s'3.:k&s'3.)    (s&s'1.,s&s'2.,s&s'3.)    1+(s&s'1.&minus.j&s'1.)
:cmt.                                                       +(s&s'2.&minus.j&s'2.)*d&s'1.
:cmt.                                                       +(s&s'3.&minus.j&s'3.)*d&s'2.*d&s'1.
:cmt. n    (j&s'1.:k&s'1.,...,j&s'n.:k&s'n.)    (s&s'1.,...,s&s'n.)    1+(s&s'1.&minus.j&s'1.)
:cmt.              +(s&s'2.&minus.j&s'2.)*d&s'1.
:cmt.              +(s&s'3.&minus.j&s'3.)*d&s'2.*d&s'1.
:cmt.              +...
:cmt.              +(s&s'n.&minus.j&s'n.)*d&s'n-1.
:cmt.               *d&s'n&minus.2.*...*d&s'1.
.if &e'&dohelp eq 0 .do begin
.cp 23
.* .box on 1 5 20 30 50
.sr c0=&INDlvl+1
.sr c1=&INDlvl+5
.sr c2=&INDlvl+20
.sr c3=&INDlvl+30
.sr c4=&INDlvl+50
.box on &c0 &c1 &c2 &c3 &c4
\n \Dimension          \Subscript  \Subscript
\  \Declarator         \Value
.box
\1 \(J1:K1)            \(S1)       \1+(S1-J1)
.box
\2 \(J1:K1,J2:K2)      \(S1,S2)    \1+(S1-J1)
\  \                   \           \ +(S2-J2)*D1
.box
\3 \(J1:K1,J2:K2,J3:K3)\(S1,S2,S3) \1+(S1-J1)
\  \                   \           \ +(S2-J2)*D1
\  \                   \           \ +(S3-J3)*D2*D1
.box
\. \.                  \.          \.
\. \.                  \.          \.
\. \.                  \.          \.
.box
\n \(J1:K1,...,Jn:Kn)  \(S1,...,Sn)\1+(S1-J1)
\  \                   \           \ +(S2-J2)*D1
\  \                   \           \ +(S3-J3)*D2*D1
\  \                   \           \ +
\  \                   \           \ +(Sn-Jn)*Dn-1*Dn-2*...*D1
.box off
.do end
.el .do begin
.millust begin
+---+--------------------+------------+----------------------------+
| n | Dimension          | Subscript  | Subscript                  |
|   | Declarator         | Value                                   |
+---+--------------------+------------+----------------------------+
| 1 | (J1:K1)            | (S1)       | 1+(S1-J1)                  |
+---+--------------------+------------+----------------------------+
| 2 | (J1:K1,J2:K2)      | (S1,S2)    | 1+(S1-J1)                  |
|   |                    |            |  +(S2-J2)*D1               |
+---+--------------------+------------+----------------------------+
| 3 | (J1:K1,J2:K2,J3:K3)| (S1,S2,S3) | 1+(S1-J1)                  |
|   |                    |            |  +(S2-J2)*D1               |
|   |                    |            |  +(S3-J3)*D2*D1            |
+---+--------------------+------------+----------------------------+
| . | .                  | .          | .                          |
| . | .                  | .          | .                          |
| . | .                  | .          | .                          |
+---+--------------------+------------+----------------------------+
| n | (J1:K1,...,Jn:Kn)  | (S1,...,Sn)| 1+(S1-J1)                  |
|   |                    |            |  +(S2-J2)*D1               |
|   |                    |            |  +(S3-J3)*D2*D1            |
|   |                    |            |  +                         |
|   |                    |            |  +(Sn-Jn)*Dn-1*Dn-2*...*D1 |
+---+--------------------+------------+----------------------------+
.millust end
.do end
.autonote Notes:
.note
n is the number of dimensions, 1 <= n <= 7.
.note
:cmt.j&s'i. is the value of the lower bound of the
Ji is the value of the lower bound of the
.us i'th
dimension.
.note
:cmt. k&s'i. is the value of the upper bound of the
Ki is the value of the upper bound of the
.us i'th
dimension.
.note
:cmt.If only the upper bound is specified, then j&s'i. = 1.
If only the upper bound is specified, then Ji = 1
.note
:cmt. s&s'i. is the integer value of the
Si is the integer value of the
.us i'th
subscript expression.
.note
:cmt. d&s'i. = k&s'i.&minus.j&s'i.+1 is the size of the
Di = Ki-Ji+1 is the size of the
.us i'th
dimension.
:cmt. If the value of the lower bound is 1, then d&s'i. = k&s'i..
If the value of the lower bound is 1, then Di = Ki.
.note
:cmt. A subscript of the form (j&s'1.,...,j&s'n.) has subscript value 1 and
A subscript of the form (J1,...,Jn) has subscript value 1 and
identifies the first element of the array.
:cmt. A subscript of the form (k&s'1.,...,k&s'n.) has subscript value equal to
A subscript of the form (K1,...,Kn) has subscript value equal to
the size of the array and identifies the last element of the array.
.endnote
.*
.section Classifying Array Declarators by Dimension Declarator
.*
.np
Array declarators can be classified according to the characteristics
of the dimension declarator.
The following sections discuss the three classifications.
.beglevel
.*
.section Constant Array Declarator
.*
.np
A
.us constant array declarator
.ix 'constant array declarator'
.ix 'array declarator' 'constant'
is one in which each of the dimension bound expressions is an integer
constant expression.
It is called a constant array declarator because the dimension bound
expressions can never change.
In the following example both
.id A(10)
and
.id B(&minus.5:5)
are constant array declarators.
.exam begin
      SUBROUTINE SQUARE( A )
      DIMENSION A(10), B(-5:5)
      .
      .
      .
      END
.exam end
.*
.section Adjustable Array Declarator
.*
.np
An
.us adjustable array declarator
.ix 'adjustable array declarator'
.ix 'array declarator' 'adjustable'
is one that contains at least one variable in all of its dimension
bound expressions.
It is called an adjustable array declarator because the dimension bound
expressions can change depending on the current value of the variables
in the dimension bound expressions.
The array name must be a dummy argument.
In the following example,
.id A(M,2*N)
is an adjustable array declarator.
If
.id SQUARE
is called with
.id M
having value 5 and
.id N
having value 10, then
the array
.id A
will be a 2-dimensional array having 5 rows and 20 columns.
.exam begin
      SUBROUTINE SQUARE( A, M, N )
      DIMENSION A(M,2*N)
      .
      .
      .
      END
.exam end
.*
.section Assumed-size Array Declarator
.*
.np
An
.us assumed-size array declarator
.ix 'assumed-size array declarator'
.ix 'array declarator' 'assumed-size'
is a constant array declarator or an adjustable array declarator whose
upper dimension bound of the last dimension is an asterisk
(e.g., A(M,N,*))
.xt on
or the integer value 1
(e.g., A(M,N,1)).
.xt off
The array name must be a dummy argument.
The value of the upper bound of the last dimension is
determined by the number of elements of the actual array argument and
is computed as follows.
First we compute the size of the dummy array.
Note that this size is really an upper bound.
.autonote
.note
If the corresponding actual array argument is a non-character array
name, the size of the dummy array is the size of the actual array.
.note
If the corresponding actual array argument is a non-character array
element name with a subscript value of
.us r
in an array of size
.us x,
the size of the dummy array is
.us x
+ 1 &minus.
.us r.
.note
If the corresponding actual argument is a character array name,
character array element or a substrung character array element which
begins at character
.us t
of an array with
.us c
characters then the size of the dummy array is INT((
.us c
+ 1 &minus.
.us t
) /
.us e)
where
.us
e
is the size of an element of the dummy array.
.endnote
.np
If the assumed-size array has dimension
.us
n
then the product of the first
.us
n
&minus. 1 dimensions must be less than or equal to the size of the array
as determined by one of the preceding rules.
The value of the assumed dimension is the largest integer
such that the product of all of
the dimensions is less than or equal to the size of the dummy array.
In the following example,
.id A(4,*)
is an assumed-size array declarator.
.exam begin
      DIMENSION B(10)
      .
      .
      .
      CALL SQUARE( B )
      .
      .
      .
      END

      SUBROUTINE SQUARE( A )
      DIMENSION A(4,*)
      .
      .
      .
      END
.exam end
.pc
By rule 1, the upper bound of the size of
.id A
is 10.
We now look for the largest integer
.us
n
such that 4 *
.us
n
is less than or equal to 10.
Clearly,
.us
n
is 2.
.id A
is therefore a 2-dimensional array with 4 rows and 2 columns.
.*
.section Allocatable Array Declarator
.*
.np
An
.us allocatable array declarator
.ix 'allocatable array declarator'
.ix 'array declarator' 'allocatable'
is one that contains no dimension bound expressions.
It is called an allocatable array declarator because the dimension
bounds are specified at run-time in an
.kw ALLOCATE
statement.
.cp 7
.exam begin
      DIMENSION A(:), B(:,:)
          .
          .
          .
      ALLOCATE( A(N) )
      ALLOCATE( B(0:4,5) )
.exam end
.np
In the previous example,
.id A(:)
is a one-dimensional allocatable array declarator
and
.id B(:,:)
is a two-dimensional allocatable array declarator.
The first
.kw ALLOCATE
statement is used to allocate the array
.id A
with bounds
.id 1:N.
The second
.kw ALLOCATE
statement is used to allocate the array
.id B
with bounds
.id 0:4
in the first dimension
and
.id 1:5
in the second dimension.
.endlevel
.*
.section Classifying Array Declarators by Array Name
.*
.np
Array declarators can also be classified according to the characteristic
of the array name.
The following sections discuss the two classifications.
.beglevel
.*
.section Actual Array Declarator
.*
.np
.ix 'actual array declarator'
.ix 'array declarator' 'actual'
An
.us actual array declarator
is one in which the array name is not a dummy argument.
All actual array declarators must also be constant array declarators.
An actual array declarator is permitted in a
.kw DIMENSION
statement, a type statement or a
.kw COMMON
statement.
.*
.section Dummy Array Declarator
.*
.np
A
.us dummy array declarator
.ix 'dummy array declarator'
.ix 'array declarator' 'dummy'
is one in which the array name is a dummy argument and hence can only
appear in a function or subroutine subprogram.
It can be a constant, adjustable or assumed-size array declarator.
A dummy array declarator can appear in a
.kw DIMENSION
statement or a type statement but not in a
.kw COMMON
statement.
It should be noted that the array declarator for a dummy array
declarator need not be the same as the array declarator of the
corresponding actual array declarator.
Also note that every array declarator in a main program must be a
constant array declarator.
.endlevel
.*
.section Use of Array Names
.*
.np
The appearance of an array name must always be as part of an array
element name except in the following cases:
.autonote
.note
in a list of dummy arguments.
For example, a subroutine that has as one of its arguments an array.
.note
in a
.kw COMMON
statement to define that array as belonging to a common block.
.note
in a type statement either as part of an array declarator or by itself
to establish the type of the array.
.note
in an array declarator in a
.kw DIMENSION
.ct , type or
.kw COMMON
statement.
.note
in an
.kw EQUIVALENCE
statement.
.note
in a
.kw DATA
statement.
.note
in the list of actual arguments when calling an external procedure.
.note
In the list of an input/output statement.
.note
as a unit identifier for an internal file in an input/output
statement.
.note
as a format identifier in an input/output statement.
.note
in a
.kw SAVE
statement.
.endnote
