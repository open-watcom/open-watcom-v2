.section DO WHILE Statement
.*
.mext begin
      DO [s[,]] WHILE (e)    [: block-label]
.mext end
.synote 15
.mnote s
is an optional statement label of an executable statement, called
the
.us terminal statement,
which follows the
.kw DO
statement in the same program unit.
.mnote e
is a logical expression or integer arithmetic expression, in which
case the result of the integer expression is compared for inequality
to the integer value 0.
.mnote block-label
is an optional block label.
.endnote
.np
The
.kw DO WHILE
statement
is an extension to the FORTRAN 77 language.
.exam begin
      X = 0.0
      DO 10 WHILE( X .LT. 100.0 )
          PRINT *, X, SQRT( X )
          X = X + 1.0
10    CONTINUE
.exam end
.np
If no statement label is present, the terminal statement of the
DO-loop must be an
.kw END DO
statement.
.exam begin
      X = 0.0
      DO WHILE( X .LT. 100.0 )
          PRINT *, X, SQRT( X )
          X = X + 1.0
      ENDDO
.exam end
.np
The following example illustrates the use of an integer arithmetic
expression.
.exam begin
      I = 10
      DO WHILE( I )
          PRINT *, I
          I = I - 1
      ENDDO
      END
.exam end
.np
The
.kw DO WHILE
statement, is similar to the
.kw DO
statement.
All nesting rules that apply to the
.kw DO
statement also apply to the
.kw DO WHILE
statement.
The difference is the way in which the looping is accomplished;
the DO-loop is executed while the logical expression of the
.kw DO WHILE
statement has a true value or until control is transferred out of the
DO-loop.
.np
For more information, see the chapter entitled :HDREF refid='fstruct'..
