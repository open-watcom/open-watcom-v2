.section Unconditional GO TO Statement
.*
.mbox begin
      GO TO s
.mbox end
.synote
.mnote s
is the statement label of an executable statement that appears in
the same program unit as the
.kw GO TO
statement.
.endnote
.exam begin
      GO TO 10
      .
      .
      .
10    S = S + 1
.exam end
.np
When the
.kw GO TO
statement is executed, control is transferred to the statement
identified by that label.
In the above example, the
.kw GO TO
statement causes execution to proceed to the statement labelled
10.
.exam begin
* An illegal GO TO statement
      GO TO 100
      .
      .
      .
100      FORMAT( 1X, 3F10.2 )
.exam end
.pc
The above example contains an illegal
.kw GO TO
statement since the statement identified by the label 100 is not
executable.
.*
.stmt GO TO (Computed)
.*
.section Computed GO TO Statement
.*
.mbox begin
      GO TO (s [,s]...) [,] i
.mbox end
.synote
.mnote i
is an integer expression.
.mnote s
is the statement label of an executable statement that appears in
the same program unit as the computed
.kw GO TO
statement.
.endnote
.np
The integer expression
.id i
is evaluated and the
.id i
.ct th label is selected for transfer of control.
If
.id i
is less than 1 or greater than the number of statement labels
in the list then execution control continues with the next
executable statement that follows the computed
.kw GO TO
statement.
.exam begin
      GO TO (110, 120, 130, 140) INDEX
100   CALL AUDIT
.exam end
.pc
In the above example, control is transferred to the statement
identified by the label 110 if
.id INDEX
has the value 1, the label 120 if
.id INDEX
has the value 2, etc.
If
.id INDEX
has a value that is negative, zero or larger than 4, control
continues with the statement labelled 100.
In this example, the integer expression consists simply of an integer
variable.
.exam begin
      GO TO (100, 200, 100, 200, 100, 200), I/10
.exam end
.pc
The above example illustrates that statement labels may be repeated
in the list and that a "," may follow the closing right parenthesis.
.*
.stmt GO TO (Assigned)
.*
.section Assigned GO TO Statement
.*
.mbox begin
      GO TO i [[,] (s [,s]...)]
.mbox end
.synote
.mnote i
is an integer variable name.
.mnote s
is the statement label of an executable statement that appears in
the same program unit as the assigned
.kw GO TO
statement.
.endnote
.np
The variable
.id i
must be defined with the value of a statement label of an
executable statement that appears in the same program unit
(see the
.kw ASSIGN
statement).
The execution of the assigned
.kw GO TO
statement causes a transfer of control to the statement that
is identified by that label.
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
*      Print both X and its square root
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
.np
If a list of statement labels is present then the statement
label assigned to
.id i
must be in the list.
If it is not in the list, an error will occur when the assigned
.kw GO TO
statement is executed.
Unlike the computed
.kw GO TO
statement, execution does not continue with the next statement.
This is demonstrated by the following example.
Note that the "," preceding the statement label list is optional.
.exam begin
* Illegal use of the assigned GO TO:
*   Statement label 100 does not appear in the statement
*   label list of the assigned GO TO statement.
      ASSIGN 100 TO ICASE
      GO TO ICASE, (110, 120, 130)
*      beginning of selections
100          PRINT *, 100
      GO TO 200
110          PRINT *, 110
      GO TO 200
120          PRINT *, 120
      GO TO 200
130          PRINT *, 130
*      end of selections
200      END
.exam end
