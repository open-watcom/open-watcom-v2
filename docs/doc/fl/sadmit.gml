.section ADMIT Statement
.*
.mext begin
      ADMIT
.mext end
.np
The
.kw ADMIT
statement is used in conjunction with the structured
.kw GUESS
statement.
The
.kw ADMIT
statement marks the beginning of an alternative block of
statements that are executed if a
.kw QUIT
statement is executed in a previous
.kw GUESS
or
.kw ADMIT
block.
.exam begin
*     Assume incorrect sex code
      GUESS
          IF( SEX .EQ. 'F' )QUIT
          IF( SEX .EQ. 'M' )QUIT
          PRINT *, 'Invalid sex code encountered'
          CALL INVSEX( SEX )
          .
          .
          .
*     Wrong assumption - sex code is fine
      ADMIT
          .
          .
          .
      END GUESS
.exam end
.np
For more information, see the chapter entitled :HDREF refid='fstruct'..
