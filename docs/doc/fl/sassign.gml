.section Statement Label Assignment (ASSIGN) Statement
.*
.mbox begin
      ASSIGN s TO i
.mbox end
.synote
.mnote s
is a statement label
.mnote i
is an integer variable name
.endnote
.np
The statement label
.id s
is assigned to the integer variable
.id i.
The statement label must appear in the same program unit
as the
.kw ASSIGN
statement.
The statement label must be that of an executable statement or a
.kw FORMAT
statement.
.np
After a statement label has been assigned to an integer variable,
that variable may only be used in an assigned
.kw GO TO
statement or as a format identifier in an input/output statement.
The integer variable must not be used in any other way (e.g., in an
arithmetic expression).
It may, however, be redefined with another statement label using the
.kw ASSIGN
statement or it may be assigned an integer value (e.g., in an
arithmetic assignment statement).
.exam begin
      INTEGER RET
      X = 0.0
      ASSIGN 100 TO RET
      GO TO 3000
100   X = X + 1
      ASSIGN 110 TO RET
      GO TO 3000
110   X = X + 1
      .
      .
      .
*     Print both X and its square root
3000  Y = SQRT( X )
      PRINT *, X, Y
      GO TO RET
.exam end
.pc
In the above example, we illustrate the use of the
.kw ASSIGN
statement and the assigned
.kw GO TO
statement to implement a "local subroutine" in a program unit.
A sequence of often-used code can be "called" using the
unconditional
.kw GO TO
statement and "return" is accomplished using the assigned
.kw GO TO
statement.
Care must be exercised to properly assign the return label value.
.exam begin
      IF( FIRST )THEN
          ASSIGN 100 TO LFRMT
      ELSE
          ASSIGN 200 TO LFRMT
      END IF
      WRITE( UNIT=5, FMT=LFRMT) X, Y, Z
100   FORMAT(1X,3F10.5)
200   FORMAT(1X,3E15.7)
.exam end
.np
It should be noted that the
.kw ASSIGN
statement does not assign the numeric value of the statement
label to the variable.
.exam begin
      ASSIGN 100 TO LABEL2
      PRINT *, LABEL2
.exam end
.pc
Try the above example; the value printed will not be 100.
.np
Consider the following example.
.exam begin
* Illegal use of a GOTO statement.
      LABEL2 = 123
      LABEL3 = LABEL2 + 10
      GO TO LABEL3
.exam end
.pc
.id LABEL3
is assigned the integer value 133.
The assigned
.kw GO TO
statement, which follows it, is illegal and a run-time error will
occur when it is executed.
.np
Statement label values are quite different from integer values and the
two should never be mixed.
In the following example, the assignment statement is illegal since it
involves an integer variable that was specified in an
.kw ASSIGN
statement.
.exam begin
* Illegal use of an ASSIGNed variable in an expression.
      ASSIGN 100 TO LABEL2
      LABEL3 = LABEL2 + 10
.exam end
.pc
Note that if the assignment statement was preceded by
.millust LABEL2 = 100
the assignment statement would have been legal.
