.section GUESS Statement
.*
.mext begin
      GUESS    [: block-label]
.mext end
.np
The
.kw GUESS
statement is an extension to the FORTRAN 77 language.
The
.kw GUESS
statement marks the beginning of a block of statements for which
a certain assumption or hypothesis has been made.
This hypothesis may be tested using logical
.kw IF
statements in conjunction with
.kw QUIT
statements.
The
.kw ADMIT
statement may be used to mark the beginning of an alternate
hypothesis.
The
.kw END GUESS
statement is used to mark the end of a series of GUESS-ADMIT blocks.
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
An optional block label may be specified with the
.kw GUESS
statement.
.np
For more information, see the chapter entitled :HDREF refid='fstruct'..
