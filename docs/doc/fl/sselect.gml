.section SELECT Statement
.*
.mext begin
      SELECT [CASE] (e) [FROM]  [: block-label]
.mext end
.np
The
.kw SELECT
statement is used in conjunction with the
.kw CASE
and
.kw END SELECT
statements.
The form of a
.kw SELECT
block is as follows:
.millust begin
    SELECT [CASE] (e) [FROM]  [: block-label]
    CASE ( case-list )
        statement (s)
    CASE ( case-list )
        statement (s)
        .
        .
        .
    CASE ( case-list )
        statement(s)
    CASE DEFAULT
        statement(s)
    END SELECT
.millust end
.synote 11
.mnote e
is an integer expression.
.ix 'case list'
.mnote case-list
is a list of one or more
.us cases
separated by commas.
A
.us case
is either
.begpoint
.point (a)
a single integer, logical or character constant expression or
.point (b)
an integer, logical or character constant expression followed by a
colon followed by another expression or the same type.
This form of a case defines a range of values consisting of all
integers or characters greater than or equal to the value of the
expression preceding the colon and less than or equal to the value of
the expression following the colon.
.endpoint
.endnote
.np
The
.kw CASE
and
.kw FROM
keywords are optional in the
.kw SELECT
statement.
An optional block label may be specified with the
.kw SELECT
statement.
.np
The case expression
.us e
is evaluated and if the result is equal to one of the values covered
by
.id case-list
then the control of execution is transferred to the associated
.kw CASE
block.
.cp 12
.exam begin
      SELECT CASE ( CH )
      CASE ( 'a' : 'z' )
          PRINT *, 'Lower case letter'
      CASE ( 'A' : 'Z' )
          PRINT *, 'Upper case letter'
      CASE ( '0' : '9' )
          PRINT *, 'Digit'
      CASE DEFAULT
          PRINT *, 'Special character'
      END SELECT
.exam end
.pc
In the above example, if the character
.id CH
is not a letter or digit then the
.kw CASE DEFAULT
block is executed.
.np
The
.kw CASE DEFAULT
statement is optional.
If it is present and the case expression is out of range (i.e., no
.kw CASE
blocks are executed) then the
.kw CASE DEFAULT
block is executed.
If it is not present and the case expression is out of range then
execution continues with the first executable statement following the
.kw END SELECT
statement.
The
.kw CASE DEFAULT
block must follow all other
.kw CASE
blocks.
.exam begin
      SELECT CASE ( I )
      CASE ( 1 )
          Y = Y + X
          X = X * 3.2
      CASE ( 2 )
          Z = Y**2 + X
      CASE ( 3 )
          Y = Y * 13. + X
          X = X - 0.213
      CASE ( 4 )
          Z = X**2 + Y**2 - 3.0
          Y = Y + 1.5
          X = X * 32.0
      CASE DEFAULT
          PRINT *, 'CASE is not in range'
      END SELECT
      PRINT *, X, Y, Z
.exam end
.np
In order to retain compatibility with earlier versions of WATCOM
FORTRAN 77 compilers, the
.kw OTHERWISE
statement may be used in place of the
.kw CASE DEFAULT
statement.
.np
For more information, see the chapter entitled :HDREF refid='fstruct'..
