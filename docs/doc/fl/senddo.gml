.section END DO Statement
.*
.mext begin
      END DO
.mext end
.np
The
.kw END DO
statement is used to terminate the range of a "structured"
.kw DO
statement.
A structured
.kw DO
statement is one in which a statement label is not present.
For more information, see the description of the structured
.kw DO
statement or the chapter entitled :HDREF refid='fstruct'..
.exam begin
      DO X = -5.1, 12.8, 0.125
          .
          .
          .
      END DO
.exam end
.exam begin
      X = -5.1
      DO WHILE( X .LE. 12.8 )
          .
          .
          .
          X = X + 0.125
      END DO
.exam end
