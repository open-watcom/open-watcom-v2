.section DO Statement
.*
.np
Two forms of the
.kw DO
statement are presented.
The second form is a &product extension to the FORTRAN 77 language.
.beglevel
.*
.section Standard DO Statement
.*
.mbox begin
      DO s [,] i = e1, e2 [, e3]
.mbox end
.synote 13
.mnote s
is the statement label of an executable statement, called the
.us terminal statement,
which follows the
.kw DO
statement in the same program unit.
.mnote i
is an integer, real, or double precision variable, called the
DO-variable.
.mnote e1, e2, e3
are each an integer, real, or double precision expression.
.endnote
.*
.section Extended DO Statement
.*
.mext begin
      DO [s[,]] i = e1, e2 [, e3]    [: block-label]
.mext end
.synote 15
.mnote s
is an optional statement label of an executable statement, called
the
.us terminal statement,
which follows the
.kw DO
statement in the same program unit.
.mnote i
is an integer, real, or double precision variable, called the
DO-variable.
.mnote e1, e2, e3
are each an integer, real, or double precision expression.
.mnote block-label
is an optional block label.
.endnote
.np
This form of the
.kw DO
statement is an extension to the FORTRAN 77 language.
If no statement label is present then the terminal statement of the
DO-loop must be an
.kw END DO
statement.
In all other respects, the rules are the same as those given for the
standard
.kw DO
statement.
.*
.section Description of DO Statement
.*
.np
The range of a DO-loop consists of all of the executable
statements that appear following the
.kw DO
statement that specifies the DO-loop, up to and including the
terminal statement of the DO-loop.
Only certain statements can be the terminal statement of a DO-loop.
See the section entitled :HDREF refid='stmtcls'. at the beginning of
this chapter for a list of these statements.
.np
Transfer of control into the range of a DO-loop from outside the
range is not permitted.
.np
A DO-loop may be executed 0 or more times.
The following sequence occurs when a
.kw DO
statement is encountered.
.begpoint
.point (i)
An
.us initial
value,
.id m1,
is calculated by evaluating expression
.id e1.
A
.us terminal
value,
.id m2,
is calculated by evaluating expression
.id e2.
An
.us incrementation
value,
.id m3,
is calculated by evaluating expression
.id e3
if it is present; otherwise
.id m3
has the value one.
If
.id e3
is specified,
.id m3
must not be zero.
The type of
.id m1, m2, and m3
is determined from the DO-variable and any conversions of
type are done as required.
.point (ii)
The DO-variable is defined with the initial value
.id m1.
.point (iii)
The iteration count (i.e., the maximum number of times that the
DO-loop will be executed) is calculated as follows:
.millust begin
    MAX( INT((m2 - m1 + m3)/m3), 0 )
.millust end
.pc
The iteration count will be zero whenever:
.millust begin
    m1 > m2 and m3 > 0, or
    m1 < m2 and m3 < 0.
.millust end
.pc
The number of times that the DO-loop is executed may be reduced
if control is transferred outside the range of the DO-loop, or if
a
.kw RETURN
or
.kw STOP
statement is executed.
.endpoint
.np
The steps involved in each iteration of the DO-loop are as
follows:
.begpoint
.point (i)
Check the iteration count.
If it is not zero then start execution of the first executable
statement of the DO-loop.
If the count is zero then iteration of the DO-loop is complete.
.point (ii)
Execute statements until the terminal statement is encountered.
During this time, the DO-variable may not be redefined.
.point (iii)
Execute the terminal statement.
Unless execution of the terminal statement causes a transfer of
control, proceed with the next step which is "incrementation"
processing.
.point (iv)
The DO-variable is incremented by the value
.id m3.
The iteration count is decremented by one.
Go back to step (i).
.endpoint
.exam begin
      DO 10 I = -5, 5
          PRINT *, I, I*I
10    CONTINUE
.exam end
.pc
In this example, the initial value is -5, the terminal value is
5, and the incrementation value is 1 (the default).
The DO-variable is
.id I.
The DO-loop is executed
.millust begin
    MAX( INT((5 - (-5) + 1)/1), 0 )
.millust end
.pc
or 11 times.
The successive values of
.id I,
inside the range of the DO-loop, are -5, -4, -3, ..., 0, 1, ...,
4, 5.
When the DO-loop is terminated, the value of
.id I
will be 6.
It should be noted that when a DO-loop variable is of type real,
the iteration count may be one less than expected.
Because of rounding errors, the value of
.mono m2 - m1 + m3
may be slightly less than the exact value and when the INT function
is applied, the resulting iteration count is one less than expected.
.exam begin
      DO 10 X = -5, 6, 2
          PRINT *, X, X*X
10    CONTINUE
.exam end
.pc
In this example, the terminal value has been changed to 6 and the
incrementation value has been changed to 2.
The DO-variable is
.id X,
a real variable.
Thus the values of
.id e1, e2
and
.id e3
are converted to type real.
The DO-loop is executed
.millust begin
    MAX( INT((6 - (-5) + 2)/2), 0 )
    MAX( INT(    13      /  2), 0 )
.millust end
.pc
or 6 times.
The successive values of
.id X,
inside the range of the DO-loop, are -5.0, -3.0, -1.0, 1.0, 3.0, 5.0.
When the DO-loop is terminated, the value of
.id X
will be 7.0.
.np
DO-loops may be nested, that is, another DO-loop may be contained
within the range of the outer DO-loop.
More than one DO-loop may have the same terminal statement.
.exam begin
      DO 10 I = -5, 5
      DO 10 J = -2, 3
10        ARRAY( I, J ) = 0.0
.exam end
.pc
This is equivalent to the following example.
.exam begin
      DO 10 I = -5, 5
          DO 20 J = -2, 3
              ARRAY( I, J ) = 0.0
20        CONTINUE
10    CONTINUE
.exam end
.np
If a
.kw DO
statement appears within the range of a DO-loop, its range must
be entirely contained within the range of the outer DO-loop.
.exam begin
* Illegal use of nested DO-loops.
      DO 20 I = -5, 5
          DO 10 J = -2, 3
              ARRAY( I, J ) = 0.0
20        CONTINUE
10    CONTINUE
.exam end
.pc
The above example is illegal since the terminal statement of the
first DO-loop precedes that of the second DO-loop.
.np
Similarly, the range of a DO-loop that appears within the range
of an IF-block, ELSE IF-block, or ELSE-block must be entirely
contained within that IF-block, ELSE IF-block, or ELSE-block,
respectively.
This rule applies to all &product structured block extensions.
.exam begin
* Illegal nesting of a DO-loop and an IF-block.
      IF( A .LT. B )THEN
          DO 10 I = 1, 5
              PRINT *, 'Iteration number', I
      END IF
              VECTOR( I ) = I
10        CONTINUE
.exam end
.pc
The above example is illegal since the range of the IF-block
must terminate after the range of the DO-loop.
Note how statement indentation helps to illustrate the problem with
this example.
.np
It is also illegal to attempt to transfer control into the range
of a DO-loop.
The following example illustrates this error.
.exam begin
* Illegal transfer into the range of a DO-loop.
      GO TO 20
      .
      .
      .
      DO 10, I = 100, 0, -1
          PRINT *, 'Counting down from 100 to 0', I
20        PRINT *, I, SQRT( FLOAT(I) )
10    CONTINUE
.exam end
.np
The following example shows a more subtle form of this error.
.exam begin
* Illegal transfer into the range of a DO-loop.
      DO 10 I = 1, 10
*         Skip row 5 of 10x10 matrix
          IF( I .EQ. 5 )GO TO 10
          DO 10 J = 1, 10
              A( I, J ) = 0.0
10    CONTINUE
.exam end
.pc
Since the
.kw CONTINUE
statement is included in the range of the inner DO-loop, an
error message is issued.
.np
.xt begin
The following example illustrates the &product structured
.kw DO
statement.
.exam begin
      DO I = -5, 5
          DO J = -2, 3
              ARRAY( I, J ) = 0.0
          END DO
      END DO
.exam end
.pc
In keeping with more modern programming practices, this feature
allows the programmer to write DO-loops without resorting to the
use of statement labels.
A well-chosen indentation style further enhances the readability of
the program.
.xt end
.np
For more information, see the chapter entitled :HDREF refid='fstruct'..
.endlevel
