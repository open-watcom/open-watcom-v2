.section END SELECT Statement
.*
.mext begin
      END SELECT
.mext end
.np
The
.kw END SELECT
statement is used in conjunction with the
.kw SELECT
statement.
The
.kw END SELECT
statement marks the end of a series of
.kw CASE
blocks.
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
For more information, see the chapter entitled :HDREF refid='fstruct'..
