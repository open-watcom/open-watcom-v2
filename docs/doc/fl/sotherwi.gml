.section OTHERWISE Statement
.*
.mext begin
      OTHERWISE
.mext end
.np
The
.kw OTHERWISE
statement is used in conjunction with the
.kw SELECT
statement.
The
.kw OTHERWISE
statement marks the start of a new
.kw CASE
block which is a series of zero or more statements ending in an
.kw END SELECT
statement.
.np
When this statement is used and the value of a
.us case expression
is not found in any
.us case list
then control of execution is transferred to the first executable
statement following the
.kw OTHERWISE
statement.
.np
The
.kw CASE DEFAULT
statement may be used in place of the
.kw OTHERWISE
statement.
.cp 12
.exam begin
      SELECT CASE ( CH )
      CASE ( 'a' : 'z' )
          PRINT *, 'Lower case letter'
      CASE ( 'A' : 'Z' )
          PRINT *, 'Upper case letter'
      CASE ( '0' : '9' )
          PRINT *, 'Digit'
      OTHERWISE
          PRINT *, 'Special character'
      END SELECT
.exam end
.pc
In the above example, if the character
.id CH
is not a letter or digit then the
.kw OTHERWISE
block is executed.
.begnote
.setptnt 0 7
.note Note:
The
.kw OTHERWISE
or
.kw CASE DEFAULT
block must follow all other
.kw CASE
blocks.
.endnote
.np
For more information, see the chapter entitled :HDREF refid='fstruct'..
