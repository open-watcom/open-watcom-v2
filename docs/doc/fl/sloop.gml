.section LOOP Statement
.*
.mext begin
      LOOP    [:block-label]
.mext end
.np
The
.kw LOOP
statement is used in conjunction with the structured
.kw END LOOP
or
.kw UNTIL
statement.
The
.kw LOOP
statement marks the beginning of a sequence of statements
which are to be repeated.
The
.kw END LOOP
or
.kw UNTIL
statement marks the end of the loop.
The LOOP-block is executed until control is transferred out of the
LOOP-block or the logical expression (or integer arithmetic
expression) of the
.kw UNTIL
statement has a true (or non-zero) value.
.np
The
.kw QUIT
statement may be used to transfer control out of a LOOP-block.
.exam begin
      LOOP
          READ *, X
          IF( X .GT. 99.0 ) QUIT
          PRINT *, X
      END LOOP
.exam end
.exam begin
      X = 1.0
      LOOP
          PRINT *, X, SQRT( X )
          X = X + 1.0
      UNTIL( X .GT. 10.0 )
.exam end
.np
An optional block label may be specified with the
.kw LOOP
statement.
.np
For more information, see the chapter entitled :HDREF refid='fstruct'..
