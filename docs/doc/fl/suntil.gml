.section UNTIL Statement
.*
.mext begin
      UNTIL (e)
.mext end
.synote
.mnote e
is a logical expression or integer arithmetic expression, in which
case the result of the integer expression is compared for inequality
to the integer value 0.
.endnote
.np
The
.kw UNTIL
statement is used in conjunction with the structured
.kw LOOP
or block
.kw WHILE
statement.
The
.kw LOOP
or block
.kw WHILE
statement marks the beginning of a sequence of statements which
are to be repeated.
The
.kw UNTIL
statement marks the end of the loop.
The LOOP-block or WHILE-block is executed until control is transferred
out of the block or the logical expression of the
.kw UNTIL
statement has a true value.
.exam begin
      X = 1.0
      LOOP
          PRINT *, X, SQRT( X )
          X = X + 1.0
      UNTIL( X .GT. 10.0 )
.exam end
.exam begin
      I = 1
      WHILE( I .LT. 100 )DO
          J = 4 * I * I
          K = 3 * I
          PRINT *, '4x**2 + 3x + 6 = ', J + K + 6
          I = I + 1
      UNTIL( ( J + K + 6 ) .GT. 100 )
.exam end
.np
For more information, see the chapter entitled :HDREF refid='fstruct'..
