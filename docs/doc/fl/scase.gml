.section CASE Statement
.*
.mext begin
      CASE cl
.mext end
.synote
.mnote cl
is a list, enclosed in parentheses, of one or more
.us cases
separated by commas, or the
.kw DEFAULT
keyword.
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
statement is used in conjunction with the
.kw SELECT
statement.
The
.kw CASE
statement marks the start of a new
.kw CASE
block which is a series of zero or more statements ending in
another
.kw CASE
statement, a
.kw CASE DEFAULT
statement, or an
.kw END SELECT
statement.
.np
A particular case value or range of values must not be contained
in more than one
.kw CASE
block.
.np
The
.kw CASE DEFAULT
statement is used to indicate a block of statements that are to be
executed when no other case is selected.
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
.np
In order to retain compatibility with earlier versions of WATCOM
FORTRAN 77 compilers, the
.kw OTHERWISE
statement may be used in place of the
.kw CASE DEFAULT
statement.
.np
For more information, see the chapter entitled :HDREF refid='fstruct'..
