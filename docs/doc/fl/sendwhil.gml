.section END WHILE Statement
.*
.mext begin
      END WHILE
.mext end
.np
The
.kw END WHILE
statement is used in conjunction with the structured
.kw WHILE
statement.
The
.kw END WHILE
statement marks the end of a sequence of statements which
are to be repeated.
The
.kw WHILE
statement marks the beginning of the WHILE-block.
The WHILE-block is executed while the logical expression (or integer
arithmetic expression) of the
.kw WHILE
statement has a true (or non-zero) value or until control is
transferred out of
the WHILE-block.
.exam begin
      X = 1.0
      WHILE( X .LT. 100 )DO
          PRINT *, X, SQRT( X )
          X = X + 1.0
      END WHILE
.exam end
.exam begin
      I = 10
      WHILE( I )DO
          PRINT *, I
          I = I - 1
      ENDWHILE
      END
.exam end
.np
For more information, see the chapter entitled :HDREF refid='fstruct'..
