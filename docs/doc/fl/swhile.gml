.section Block WHILE Statement
.*
.mext begin
      WHILE (e) DO    [: block-label]
.mext end
.synote
.mnote e
is a logical expression or integer arithmetic expression, in which
case the result of the integer expression is compared for inequality
to the integer value 0.
.endnote
.np
The block
.kw WHILE
statement is used in conjunction with the structured
.kw END WHILE
or
.kw UNTIL
statement.
The block
.kw WHILE
statement marks the beginning of a sequence of statements which
are to be repeated.
The
.kw END WHILE
or
.kw UNTIL
statement marks the end of the WHILE-block.
The WHILE-block is executed while the logical expression of the
.kw WHILE
statement has a true value or until control is transferred out of
the WHILE-block.
.exam begin
      X = 1.0
      WHILE( X .LT. 100 )DO
          PRINT *, X, SQRT( X )
          X = X + 1.0
      END WHILE
.exam end
.exam begin
      I = 1
      WHILE( I .LT. 100 )DO
          J = 4 * I * I
          K = 3 * I
          PRINT *, '4x**2 + 3x + 6 = ', J + K + 6
          I = I + 1
      UNTIL( ( J + K + 6 ) .GT. 100 )
      END
.exam end
.np
An optional block label may be specified with the
.kw WHILE
statement.
.np
For more information, see the chapter entitled :HDREF refid='fstruct'..
.*
.stmt WHILE
.*
.section WHILE Statement
.*
.mext begin
      WHILE (e) stmt
.mext end
.synote 6
.mnote e
is a logical expression.
.mnote stmt
is an executable statement.
Only certain executable statements are allowed.
See the section entitled :HDREF refid='stmtcls'. at the
beginning of this chapter for a list of allowed statements.
.endnote
.np
This form of the
.kw WHILE
statement allows an executable statement to be repeatedly executed
until the logical expression
.us e
is false.
.exam begin
      I = 0
      WHILE( I .LE. 100 ) CALL PRTSQR( I )
      END

      SUBROUTINE PRTSQR( J )
      PRINT *, J, J**2
      J = J + 1
      END
.exam end
.pc
In the above example, the subroutine
.id PRTSQR
is called again and again until the value of
.id I
has been incremented beyond 100.
Note that the subroutine increments its argument thereby guaranteeing
that the program will eventually stop execution.
.np
For more information, see the chapter entitled :HDREF refid='fstruct'.
Control Statements".
