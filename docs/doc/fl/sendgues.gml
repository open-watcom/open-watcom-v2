.section END GUESS Statement
.*
.mext begin
      END GUESS
.mext end
.np
The
.kw END GUESS
statement is used in conjunction with the structured
.kw GUESS
statement.
The
.kw END GUESS
statement marks the end of a series of GUESS-ADMIT blocks.
.exam begin
      CHARACTER CH
      READ *, CH
      GUESS
          IF( CH .LT. 'a' )QUIT
          IF( CH .GT. 'z' )QUIT
          PRINT *, 'Lower case letter'
      ADMIT
          IF( CH .LT. 'A' )QUIT
          IF( CH .GT. 'Z' )QUIT
          PRINT *, 'Upper case letter'
      ADMIT
          IF( CH .LT. '0' )QUIT
          IF( CH .GT. '9' )QUIT
          PRINT *, 'Digit'
      ADMIT
          PRINT *, 'Special character'
      END GUESS
      END
.exam end
.np
For more information, see the chapter entitled :HDREF refid='fstruct'..
