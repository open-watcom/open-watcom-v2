.section IMPLICIT Statement
.*
.np
&product supports three forms of the
.kw IMPLICIT
statement.
The second and third forms are extensions to the FORTRAN 77 language.
.*
.beglevel
.*
.section Standard IMPLICIT Statement
.*
.mbox begin
      IMPLICIT type (a [,a] ...) [,type (a [,a] ...)]...
.mbox end
.synote 6
.mnote type
is one of
.mono LOGICAL, INTEGER, REAL, DOUBLE PRECISION, COMPLEX
or
.mono CHARACTER[*len].
.mnote a
is either a single letter or a range of letters denoted by separating the
first letter in the range from the last letter in the range by
a minus sign.
.mnote len
is the length of the character entities and is a positive unsigned
integer constant or a positive integer constant expression enclosed in
parentheses.
If
.id len
is not specified, the length is 1.
.endnote
.*
.section Extended IMPLICIT Statement
.*
.mext begin
      IMPLICIT type[*len] (a [,a] ...)
             [,type[*len] (a [,a] ...)] ...
.mext end
.synote 6
.mnote type
is one of
.mono LOGICAL, INTEGER, REAL, DOUBLE PRECISION, COMPLEX
or
.mono CHARACTER.
.mnote len
is a positive unsigned integer constant or a positive integer constant
expression enclosed in parentheses.
If
.id type
is
.mono CHARACTER
then
.mono (*)
is also allowed.
The possible values of
.id len
are as follows:
.autonote
.note
If
.id type
is
.mono LOGICAL
then
.id len
can be 1 or 4.
The default is 4.
.note
If
.id type
is
.mono INTEGER
then
.id len
can be 1, 2 or 4.
The default is 4.
.note
If
.id type
is
.mono REAL
then
.id len
can be 4 or 8.
The default is 4.
.note
If
.id type
is
.mono DOUBLE PRECISION
then
.id len
cannot be specified.
.note
If
.id type
is
.mono COMPLEX
then
.id len
can be 8 or 16.
The default is 8.
.note
If
.id type
is
.mono CHARACTER
then
.id len
can be
.mono (*)
or any positive integer.
.endnote
.endnote
.*
.section IMPLICIT NONE Statement
.*
.mext begin
      IMPLICIT NONE
.mext end
.*
.section Description of IMPLICIT Statement
.*
.np
The
.kw IMPLICIT
statement defines the default type and length for all variables,
arrays, symbolic constants, external functions and statement
functions that begin with any letter that has appeared in an
.kw IMPLICIT
statement as a single letter or as a member of a range of
letters.
.np
The following example specifies that any name beginning with the
letters
.id A,
.id D,
.id E,
.id F
or
.id G
will have default a default type of integer and any name beginning with
the letters
.id X,
.id Y
or
.id Z
will have a default type of character and length 3.
.exam begin
      IMPLICIT INTEGER (A,D-G), CHARACTER*3 (X-Z)
.exam end
.np
.cp 8
.xt begin
The next example illustrates the extended form of the
.kw IMPLICIT
statement.
.exam begin 5
      IMPLICIT INTEGER*2 (A,B), LOGICAL*1 (C-F)
      IMPLICIT COMPLEX*16 (X,Y,Z), REAL*8 (P)
.exam end
.xt end
.np
.cp 15
.xt begin
Specifying NONE in the
.kw IMPLICIT
statement will cause &product to issue an error when a symbol is used
and has not appeared in a type specification statement.
.exam begin 5
* Referencing X will cause an error
      IMPLICIT NONE
      X = 13143.383
.exam end
.pc
In the above example, the
.kw IMPLICIT
statement specifies that the type of all symbols must be explicitly
declared in a type specification statement.
The assignment statement will cause an error since the type of
.id X
has not been explicitly declared.
.xt end
.autonote Notes:
.setptnt 0 5
.note
The implicit type set by an
.kw IMPLICIT
statement may be overridden or confirmed for any variable, array,
symbolic constant, external function or statement function name
by its appearance in a type statement.
The default length specification may also be overridden or
confirmed in a type statement.
.exam begin
      IMPLICIT CHARACTER*10 (S-U)
      IMPLICIT INTEGER*2 (P)
      CHARACTER STRING
      INTEGER POINTS
.exam end
.pc
In the above example, the variable
.id STRING
is of type character but its length is 1 since it has appeared in
a
.kw CHARACTER
statement which has a default length of 1.
Also, the variable
.id POINTS
is of type integer but its length is 4 since it has appeared in
an
.kw INTEGER
statement which has a default length of 4.
.note
A letter cannot appear more than once as a single letter or be
included in a range of letters in all
.kw IMPLICIT
statements in a program unit.
.note
An
.kw IMPLICIT
statement applies only to the program unit that contains it.
.note
The
.kw IMPLICIT
statement does not change the type of intrinsic functions.
.note
A program unit can contain more than one
.kw IMPLICIT
statement.
.note
Within the specification statements of a program unit,
.kw IMPLICIT
statements must precede all other specification statements except
.kw PARAMETER
statements.
.note
.xt begin
The
.kw IMPLICIT NONE
statement is allowed only once in a program unit.
Furthermore, no other
.kw IMPLICIT
statement can be specified in the program unit containing an
.kw IMPLICIT NONE
statement.
.xt end
.endnote
.*
.endlevel
