.chap *refid=fstruct Program Structure Control Statements
.*
.if &e'&dohelp eq 0 .do begin
.section Introduction
.do end
.*
.np
The use of structured programming statements has been found to
encourage better programming and design practices among beginners, and
aids the more experienced programmer in writing error-free programs.
.np
The format of these statements and their blocks is illustrated below.
Following this, the use and meaning of each statement is described and
illustrated with examples.
In each of these illustrations, the blocks are denoted by
.mono statement(s)
and are delimited by control statements.
.np
.xt begin
In the descriptions,
.mono logical-expression
can also be an integer expression, in which case the result of the
integer expression is compared for inequality to the integer value 0.
.exam begin
    IF( LEN - 1 )THEN
.exam end
.pc
In the preceding example, the expression
.mono LEN - 1
is interpreted as
.mono LEN - 1 .NE. 0.
.xt end
.*
.section IF - ELSE - END IF
.*
.np
The
.kw ELSE
portion of this construct is optional, thus there are two possible
formats.
.mext begin
(a) IF( logical-expression )THEN    [: block-label]
        statement(s)
    END IF

(b) IF( logical-expression )THEN    [: block-label]
        statement(s)
    ELSE
        statement(s)
    END IF
.mext end
.np
This construct is an enhancement of the FORTRAN logical
.kw IF
statement.
If the value of the parenthesized logical expression is true in (a),
the block of statements following the
.kw IF
statement is executed, after which control passes to the statement
following the
.kw END IF
statement; otherwise, control will pass directly to the statement
following the
.kw END IF
statement.
When the
.kw ELSE
statement is used and the logical expression is true, the block of
statements between the
.kw IF
and the
.kw ELSE
statements is executed and then control passes to the statement
following the
.kw END IF
statement; otherwise the block of statements following
.kw ELSE
statement is executed and then control passes to the statement
following the
.kw END IF
statement.
.np
.xt begin
An optional block label may be specified with the
.kw IF
statement (see the
.kw CYCLE
.ct ,
.kw EXIT
or
.kw QUIT
statement for more information).
.xt end
.np
Examples follow which illustrate the use of the two formats.
.keep 10
.exam begin
      IF( I .EQ. 0 )THEN
          PRINT *, 'I IS ZERO'
          I = 1
      END IF
.exam end
.np
If variable
.id I
is zero when the
.kw IF
statement is executed, the string
.mono I IS ZERO
will be printed, variable
.id I
will be assigned the value 1, and the statement following the
.kw END IF
will be executed.
If variable
.id I
is not zero when the
.kw IF
statement is executed, control will pass to the statement following
the
.kw END IF
statement.
.keep 10
.exam begin
      IF( A .GT. B )THEN
          PRINT *, 'A GREATER THAN B'
          A = A - B
      ELSE
          PRINT *, 'A NOT GREATER THAN B'
      END IF
.exam end
.np
If the value of variable
.id A
is greater than the value of variable
.id B
when this
.kw IF
statement is executed, the string
.mono A GREATER THAN B
will be printed and variable
.id A
will be assigned the value of the expression
.mono A - B.
Control will then pass to the statement following the
.kw END IF
statement.
.np
If the value of variable
.id A
is not greater than the value of
variable
.id B
when the
.kw IF
statement is executed, the string
.mono A NOT GREATER THAN B
will be printed and control will pass to the statement following the
.kw END IF
statement.
.*
.keep 19
.section ELSE IF
.*
.np
A further enhancement of the IF-THEN-ELSE construct is the
.kw ELSE IF
statement which may be used in the following two formats:
.mext begin
(a) IF( logical-expression-1 )THEN    [: block-label]
        statement(s)
    ELSE IF( logical-expression-2 )THEN
        statement(s)
    ...
    END IF

(b) IF( logical-expression-1 )THEN    [: block-label]
        statement(s)
    ELSE IF( logical-expression-2 )THEN
        statement(s)
    ...
    ELSE
        statement(s)
    END IF
.mext end
.np
The presence of the "..." in the above formats indicates that the
.kw ELSE IF
statement may be repeated as often as desired.
If the value of
.id logical-expression-1
is true in case (a), the block of statements following the
.kw IF
statement up to the first
.kw ELSE IF
statement is executed, after which control passes to the statement
following the
.kw END IF
statement; otherwise, control will pass to the first
.kw ELSE IF
statement.
If the value of
.id logical-expression-2
is true, the block of statements following the first
.kw ELSE IF
statement up to the next
.kw ELSE IF
statement or
.kw END IF
statement is executed, after which control passes to the statement
following the
.kw END IF
statement; otherwise, control will pass to the next
.kw ELSE IF
statement, if there is one, or directly to the statement following the
.kw END IF
statement.
When the
.kw ELSE
statement is used, as in case (b), and the values of all the logical
expressions in the
.kw IF
and
.kw ELSE IF
statements are false, the block of statements following the
.kw ELSE
statement is executed and then control passes to the statement
following the
.kw END IF
statement.
.xt on
An optional block label may be specified with the
.kw IF
statement (see the
.kw CYCLE
.ct ,
.kw EXIT
or
.kw QUIT
statement for more information).
.xt off
.np
Examples follow which illustrate the use of the two formats.
.keep 10
.exam begin
      IF( I .EQ. 0 )THEN
          PRINT *, 'I IS ZERO'
      ELSE IF( I .GT. 0 )THEN
          PRINT *, 'I IS GREATER THAN ZERO'
      END IF
.exam end
.np
If variable
.id I
is zero when the
.kw IF
statement is executed, the string
.mono I IS ZERO
will be printed and the statement following the
.kw END IF
statement will be executed.
If variable
.id I
is not zero when the
.kw IF
statement is executed, control will pass to the
.kw ELSE IF
statement.
If variable
.id I
is greater than zero, the string
.mono I IS GREATER THAN ZERO
will be printed and the statement following the
.kw END IF
statement will be executed.
If variable
.id I
is less than zero then nothing would be printed and control passes
from the
.kw ELSE IF
statement to the statement following the
.kw END IF
statement.
.keep 10
.exam begin
      IF( A .GT. B )THEN
          PRINT *, 'A GREATER THAN B'
          A = A - B
      ELSE IF( A .LT. B )THEN
          PRINT *, 'A LESS THAN B'
          A = B - A
      ELSE
          PRINT *, 'A EQUAL TO B'
          A = 0.0
      END IF
.exam end
.np
If the value of variable
.id A
is greater than the value of variable
.id B
when the
.kw IF
statement is executed, the string
.mono A GREATER THAN B
will be printed and variable
.id A
will be assigned the value of the expression
.mono A - B.
Control will then pass to the statement following the
.kw END IF
statement.
.np
If the value of variable
.id A
is not greater than the value of variable
.id B
when the
.kw IF
statement is executed, control passes to the
.kw ELSE IF
statement.
If the value of variable
.id A
is less than the value of variable
.id B,
the string
.mono A LESS THAN B
will be printed and variable
.id A
will be assigned the value of the expression
.mono B - A.
Control will then pass to the statement following the
.kw END IF
statement.
.np
If the value of variable
.id A
is not less than the value of variable
.id B
when the
.kw ELSE IF
statement is executed, the string
.mono A EQUAL TO B
will be printed and variable
.id A
will be assigned the value zero.
Control will pass to the statement following the
.kw END IF
statement.
.*
.keep 10
.section DO - END DO
.*
.mext begin
  DO init-expr,end-value[,inc-value] [: block-label]
      statement(s)
  END DO
.mext end
.np
This extension to FORTRAN 77 allows the creation of DO-loops without the
introduction of statement numbers.
An optional block label may be specified (see the
.kw CYCLE
.ct ,
.kw EXIT
or
.kw QUIT
statement for more information).
The
.kw END DO
statement is used to indicate the end of the range of its
corresponding
.kw DO
statement.
A statement number may not be specified in the corresponding
.kw DO
statement.
Nested DO-loops of this form require separate
.kw END DO
statements to terminate the range of the corresponding
.kw DO
statement.
Since a statement number may appear on the
.kw END DO
statement, the number may be used to terminate outer DO-loops.
This is not a recommended practice (a
.kw CONTINUE
statement or a structured
.kw DO
statement should be used).
A transfer of control from within the DO-loop to a statement number on
the
.kw END DO
statement is treated in the same manner as if the word
.kw CONTINUE
had been used instead of
.kw END DO
.ct .li .
.np
Some examples follow.
.keep 10
.exam begin
      DO I = 1, 3
          DO J = 1, 5
              PRINT *, MATRIX( I, J )
          END DO
      END DO
.exam end
.np
The above is equivalent to the following example which uses statement
numbers.
.keep 10
.exam begin
      DO 10 I = 1, 3
          DO 20 J = 1, 5
              PRINT *, MATRIX( I, J )
20        CONTINUE
10    CONTINUE
.exam end
.np
The next example demonstrates the use of a
.kw GO TO
statement to control execution of all or part of a DO-loop.
.keep 10
.exam begin
      DO I = 1, 3
          DO J = 1, 5
              PRINT *, 'INNER LOOP - J=', J
              IF( J .LE. 3 )GO TO 20
              PRINT *, 'J > 3'
20        END DO
          PRINT *, 'OUTER LOOP - J=', J
      END DO
.exam end
.np
A result of this example is that the character string
.mono J > 3
is printed 6 times (i.e., twice for each iteration of the outer loop).
Of course there is a much better way of coding this algorithm using
the IF-END IF construct.
The example is included to illustrate the behaviour of transfers of
control to an
.kw END DO
statement.
The following example is an equivalent algorithm to the one above but
the intent is much clearer.
.keep 10
.exam begin
      DO I = 1, 3
          DO J = 1, 5
              PRINT *, 'INNER LOOP - J=', J
              IF( J .GT. 3 )THEN
                  PRINT *, 'J > 3'
              END IF
          END DO
          PRINT *, 'OUTER LOOP - J=', J
      END DO
.exam end
.*
.keep 10
.section DO WHILE - END DO
.*
.mext begin
  DO WHILE (e) [: block-label]
      statement(s)
  END DO
.mext end
.np
This extension to FORTRAN 77 allows the creation of DO-loops without
iterative techniques.
Instead, the DO-loop is executed while the parenthesized expression is
true.
The logical expression is evaluated before entry to the DO-loop.
If the value is false, control is transferred to the statement following
the
.kw END DO
statement.
If the logical expression if true, the statements of the DO-loop are
executed.
When the
.kw END DO
statement is reached, the expression is re-evaluated and program control
proceeds as previously described.
An optional block label may be specified (see the
.kw CYCLE
.ct ,
.kw EXIT
or
.kw QUIT
statement for more information).
.np
An optional statement number can be specified after the
.kw DO
keyword.
When the
.kw END DO
statement is used to indicate the end of the range of its
corresponding
.kw DO WHILE
statement, a statement number may not be specified.
.np
Some examples follow.
.keep 10
.exam begin
      I = 1
      DO WHILE( I .LE. 3 )
          J = 1
          DO WHILE( J .LE. 5 )
              PRINT *, MATRIX( I, J )
          END DO
      END DO
.exam end
.np
The above is equivalent to the following example which uses statement
numbers.
.keep 10
.exam begin
      I = 1
      DO 10 WHILE( I .LE. 3 )
          J = 1
          DO 20 WHILE( J .LE. 5 )
              PRINT *, MATRIX( I, J )
20        CONTINUE
10    CONTINUE
.exam end
.np
The next example demonstrates the use of a
.kw GO TO
statement to control execution of all or part of a DO-loop.
.keep 15
.exam begin
      I = 1
      DO WHILE( I .LE. 3 )
          J = 1
          DO WHILE( J .LE. 5 )
              PRINT *, 'INNER LOOP - J=', J
              IF( J .LE. 3 )GO TO 20
              PRINT *, 'J > 3'
20        END DO
          PRINT *, 'OUTER LOOP - J=', J
      END DO
.exam end
.np
A result of this example is that the character string
.mono J > 3
is printed 6 times (i.e., twice for each iteration of the outer loop).
Of course there is a much better way of coding this algorithm using
the IF-END IF construct.
The example is included to illustrate the behaviour of transfers of
control to an
.kw END DO
statement.
The following example is an equivalent algorithm to the one above but
the intent is much clearer.
.keep 10
.exam begin
      I = 1
      DO WHILE( I .LE. 3 )
          J = 1
          DO WHILE( J .LE. 5 )
              PRINT *, 'INNER LOOP - J=', J
              IF( J .GT. 3 )THEN
                  PRINT *, 'J > 3'
              END IF
          END DO
          PRINT *, 'OUTER LOOP - J=', J
      END DO
.exam end
.*
.keep 10
.section LOOP - END LOOP
.*
.mext begin
  LOOP    [: block-label]
      statement(s)
  END LOOP
.mext end
.np
This extension to FORTRAN 77 causes the statements between the
.kw LOOP
and
.kw END LOOP
statements to be repeated until control is transferred out of
the loop, usually by an
.kw EXIT
or
.kw QUIT
statement.
An optional block label may be specified (see the
.kw CYCLE
.ct ,
.kw EXIT
or
.kw QUIT
statement for more information).
An example follows:
.keep 10
.exam begin
      LOOP
          READ *, X
          IF( X .EQ. 99.0 )EXIT
          PRINT *, X
      END LOOP
.exam end
.np
The above statements cause values to be read and printed, one to
a line, until the value 99.0 is read.
When variable
.id X
has this value, the logical expression in the
.kw IF
statement evaluates as true and the
.kw EXIT
statement causes a transfer of control to the statement following the
.kw END LOOP
statement.
The
.kw EXIT
statement is discussed in more detail in a later section.
.*
.keep 10
.section WHILE - END WHILE
.*
.mext begin
  WHILE( logical-expression )DO      [: block-label]
      statement(s)
  END WHILE
.mext end
.np
This extension to FORTRAN 77 causes its block of code to be executed
repeatedly while the parenthesized logical expression is true.
The logical expression is evaluated before entry to the block.
If the value is false, control passes to the statement following the
.kw END WHILE
statement.
If the logical expression is true, the statements of the block are
executed.
When the
.kw END WHILE
statement is reached, the
.kw WHILE
logical expression is re-evaluated and the above program control
decisions are repeated.
.* The keyword
.* .kw DO
.* following the right parenthesis is optional.
An optional block label may be specified (see the
.kw CYCLE
.ct ,
.kw EXIT
or
.kw QUIT
statement for more information).
An example follows:
.keep 10
.exam begin
      WHILE( J .GT. 0 )DO
          A(J) = B(I + J)
          J = J - 1
      END WHILE
.exam end
.np
If variable
.id J
is zero or negative when the
.kw WHILE
statement is executed, the
.kw WHILE
block of code will be by-passed and the statement following the
.kw END WHILE
statement will be executed.
.np
If variable
.id J
is greater than zero when the
.kw WHILE
statement is executed, the
.kw WHILE
block will be executed repeatedly until
.id J
becomes equal to zero.
The effect of this loop will be to assign values to elements of array
.id A
from array
.id B,
starting with the element of
.id A
corresponding to the initial value of variable
.id J
and working backwards down the array to element 1.
.*
.keep 10
.section WHILE - Executable-statement
.*
.mext begin
  WHILE( logical-expression )stmt
.mext end
.synote 5
.mnote stmt
is an executable statement.
Only certain executable statements are are allowed.
See the section entitled :HDREF refid='stmtcls'. in the chapter
entitled :HDREF refid='fstats' page=no. for a list of allowable
statements.
.endnote
.np
This control statement is another form of the WHILE construct.
.keep 5
.exam begin
      WHILE( I .GT. 0 )EXECUTE A
.exam end
.np
When this statement is executed, if the logical expression is not
true, control passes to the next statement.
If the expression is true, REMOTE-block
.id A
(assumed to be defined elsewhere in the program unit) is executed, and
the logical expression is re-evaluated.
This is repeated until the logical expression, when evaluated, is
false; control then passes to the next statement.
.*
.keep 10
.section UNTIL
.*
.mext begin
  LOOP    [: block-label]
      statement(s)
  UNTIL( logical-expression )
.mext end
.pc
or
.mext begin
  WHILE( logical-expression )DO      [: block-label]
      statement(s)
  UNTIL( logical-expression )
.mext end
.np
The
.kw UNTIL
statement, an extension to FORTRAN 77, may be combined with either a
.kw LOOP
or
.kw WHILE
statement by replacing the
.kw END LOOP
or
.kw END WHILE
statement.
It provides a way of specifying a condition to be tested at the end of
each iteration of a loop, which will determine whether or not the loop
is repeated.
After all of the statements in the block have been executed, the
logical expression in the
.kw UNTIL
statement is evaluated.
If the result of the condition is false, the loop is repeated;
otherwise, control passes to the statement following the
.kw UNTIL
statement.
.np
In the following example, the statements between the
.kw LOOP
and the
.kw UNTIL
statements are executed until the value of variable
.id X
is greater than 10.0.
.keep 10
.exam begin
      X = 1.0
      LOOP
          PRINT *, X, SQRT( X )
          X = X + 1.0
      UNTIL( X .GT. 10.0 )
.exam end
.*
.keep 17
.section SELECT - END SELECT
.*
.mext begin
  SELECT [CASE] (e) [FROM]  [: block-label]
  CASE ( case-list )
      statement (s)
  CASE ( case-list )
      statement (s)
      .
      .
      .
  CASE DEFAULT
      statement(s)
  END SELECT
.mext end
.synote 8
.mnote case-list
is a list of one or more
.us cases
separated by commas.
A
.us case
is either
.begpoint
.point (a)
a single integer, logical or character constant expression or
.point (b)
an integer, logical or character constant expression followed by a
colon followed by another expression or the same type.
This form of a case defines a range of values consisting of all
integers or characters greater than or equal to the value of the
expression preceding the colon and less than or equal to the value of
the expression following the colon.
.endpoint
.endnote
.np
The
.kw SELECT
construct, an extension to FORTRAN 77, is similar in concept to the
FORTRAN computed
.kw GO TO
statement.
It allows one of a number of blocks of code (case blocks) to be
selected for execution by means of an integer expression in the
.kw SELECT
statement.
.np
The
.kw SELECT
statement keywords,
.kw CASE
and
.kw FROM
.ct , are optional.
The
.kw SELECT
statement may contain a block label (see the
.kw CYCLE
.ct ,
.kw EXIT
or
.kw QUIT
statements for more information).
.np
Each block must be started with a
.kw CASE
statement; however, the last block may begin with a
.kw CASE DEFAULT
statement.
The
.kw CASE DEFAULT
block is optional.
In order to retain compatibility with earlier versions of WATCOM
FORTRAN 77 compilers, the
.kw OTHERWISE
statement may be used in place of the
.kw CASE DEFAULT
statement.
The last block is ended by the
.kw END SELECT
statement.
The number of case blocks is optional, from one to many; however, it
is recommended that the SELECT construct not be used for fewer than 3
case blocks.
The conditional execution of one or two blocks of code is handled more
efficiently by the IF-THEN-ELSE construct.
.np
A particular case value or range of values must not be contained
in more than one CASE-block.
For example, the following is illegal:
.keep 10
.exam begin
* Illegal SELECT block - case value in more
* than one CASE block.
      SELECT CASE ( I - 3 )
      CASE (1,3,7:10)
          statement(s)
      CASE (5,6,8)
          statement(s)
      CASE (-3:-2+4)
          statement(s)
      END SELECT
.exam end
.pc
The second CASE-block includes 8 which is already handled by the
first CASE-block.
As well, the third CASE-block handles cases &minus.3, &minus.2, &minus.1, 0, 1, 2 but
the first CASE-block also handles case 1.
Thus the second and third CASE-ranges are in error.
.np
When the
.kw SELECT
statement case expression is evaluated as
.us i,
the case block whose range contains
.us i
is executed and control passes to the statement following the
.kw END SELECT
statement.
If no range contains
.us i,
control is transferred to the statement following the
.kw CASE DEFAULT
statement, if one is specified.
If the
.kw CASE DEFAULT
block is omitted and the case expression is out of range when the
.kw SELECT
statement is executed (that is, none of the CASE-blocks handles the
particular expression value), control is passed to the statement
following the
.kw END SELECT
statement and none of the CASE-blocks is executed.
.keep 20
.exam begin
      SELECT CASE ( I )
      CASE (1)
          Y = Y + X
          X = X * 3.2
      CASE (2)
          Z = Y**2 + X
          PRINT *, X, Y, Z
      CASE (3)
          Y = Y * 13. + X
          X = X - 0.213
      CASE (4)
          Z = X**2 + Y**2 - 3.0
          Y = Y + 1.5
          X = X * 32.0
          PRINT *, 'CASE 4', X, Y, Z
      END SELECT
.exam end
.pc
This example will execute in the manner described below for each of
the possible values of variable
.id I.
.begpoint
.point (i)
.id I
is zero or negative:
.br
- control will pass to the statement after the
.kw END SELECT
statement
.point (ii)
.id I = 1:
.br
- the value of
.id X
will be added to
.id Y
.br
-
.id X
will be multiplied by 3.2
.br
- control will pass to the statement after the
.kw END SELECT
statement
.point (iii)
.id I = 2:
.br
-
.id Z
will be assigned the value of the expression
.mono Y**2 + X
.br
- the values of
.id X, Y
and
.id Z
will be printed
.br
- control will pass to the statement after the
.kw END SELECT
statement
.point (iv)
.id I = 3:
.br
-
.id Y
will be assigned the value of the expression
.mono Y * 13. + X
.br
- 0.213 will be subtracted from
.id X
.br
- control will pass to the statement after the
.kw END SELECT
statement
.br
.point (v)
.id I = 4:
.br
-
.id Z, Y
and
.id X
will be assigned new values
.br
- the string
.mono CASE 4,
followed by the values of
.id X, Y
and
.id Z
will be printed
.br
- control will pass to the statement after the
.kw END SELECT
statement
.point (vi)
.id I = 5, 6, ...:
.br
- control will pass to the statement after the
.kw END SELECT
statement
.endpoint
.np
.kw CASE DEFAULT
allows a block of code to be specified for execution when the
.kw SELECT
expression is out of range.
It must follow all CASE-blocks and thus is ended by the
.kw END SELECT
statement.
The
.kw CASE DEFAULT
statement terminates the previous and last CASE-block.
Note that only one
.kw CASE DEFAULT
block may be specified in a SELECT construct.
.np
If a
.kw CASE DEFAULT
block were included in the above example, it would be executed in
cases (i) and (vi) of the description.
After a
.kw CASE DEFAULT
block is executed, control then passes to the statement after the
.kw END SELECT
statement.
.np
Empty or null case blocks are permitted (that is, two
.kw CASE
statements with no statements between).
The net result of executing a null CASE-block is to effectively bypass
the SELECT construct.
.keep 20
.exam begin
      SELECT CASE ( I * 4 - J )
      CASE (-10 : -5)
          PRINT *,'First case:'
          PRINT *,'-10 <= I*4-J <= -5'
      CASE (-4 : 2)
          PRINT *,'Second case:'
          PRINT *,'-4 <= I*4-J <= 2'
      CASE (3, 5, 7)
          PRINT *,'Third case:'
          PRINT *,'I*4-J is one of 3, 5 or 7'
      CASE (4, 6, 8:10)
          PRINT *,'Fourth case:'
          PRINT *,'I*4-J is one of 4, 6, 8, 9 or 10'
      CASE DEFAULT
          PRINT *,'All other cases:'
          PRINT *,'I*4-J < -10 or I*4-J > 10'
      END SELECT
.exam end
.pc
This example will execute in the manner described below for each
of the possible values of expression
.mono I*4-J.
.begpoint
.point (i)
expression < &minus.10
.br
- control will pass to the statement after the
.kw CASE DEFAULT
statement
.br
- the string
.mono All other cases:
will be printed
.br
- the string
.mono I*4-J < -10 or I*4-J > 10
will be printed
.point (ii)
&minus.10 <= expression <= &minus.5:
.br
- control will pass to the statement after the first
.kw CASE
statement
.br
- the string
.mono First case:
will be printed
.br
- the string
.mono -10 <= I*J-4 <= -5
will be printed
.br
- control will pass to the statement after the
.kw END SELECT
statement
.point (iii)
&minus.4 <= expression <= 2:
.br
- control will pass to the statement after the second
.kw CASE
statement
.br
- the string
.mono Second case:
will be printed
.br
- the string
.mono -4 <= I*J-4 <= 2
will be printed
.br
- control will pass to the statement after the
.kw END SELECT
statement
.point (iv)
expression = 3, 5 or 7:
.br
- control will pass to the statement after the third
.kw CASE
statement
.br
- the string
.mono Third case:
will be printed
.br
- the string
.mono I*J-4 is one of 3, 5 or 7
will be printed
.br
- control will pass to the statement after the
.kw END SELECT
statement
.point (v)
expression = 4, 6, 8, 9 or 10:
.br
- control will pass to the statement after the fourth
.kw CASE
statement
.br
- the string
.mono Fourth case:
will be printed
.br
- the string
.mono I*J-4 is one of 4, 6, 8, 9 or 10
will be printed.
.br
- control will pass to the statement after the
.kw END SELECT
statement
.point (vi)
expression > 10:
.br
- control will pass to the statement after the
.kw CASE DEFAULT
statement
.br
- the string
.mono All other cases:
will be printed
.br
- the string
.mono I*4-J < -10 or I*4-J > 10
will be printed
.endpoint
.*
.keep 10
.section EXECUTE and REMOTE BLOCK
.*
.mext begin
  EXECUTE   name
      .
      .
      .
  REMOTE BLOCK name
      statement(s)
  END BLOCK
.mext end
.synote 5
.mnote name
is a valid FORTRAN symbolic name.
.endnote
.np
The
.kw EXECUTE
statement, an extension to FORTRAN 77, allows a named block of code to
be executed.
The named block of code may be defined anywhere in the same program
unit and is delimited by the
.kw REMOTE BLOCK
and
.kw END BLOCK
statements.
Executing a REMOTE-block is similar in concept to calling a
subroutine, with the advantage that shared variables do not need to be
placed in a common block or passed in an argument list.
In addition there is less overhead involved in executing a
REMOTE-block than in calling a subroutine (in both amount of object
code and execution time).
When execution of the REMOTE-block is complete, control returns to the
statement following the
.kw EXECUTE
statement which invoked it.
.np
This feature is helpful in avoiding duplication of code for a function
(such as I/O) required in a number of places throughout a program.
It can also be an aid to writing a well-structured program.
.np
Each REMOTE-block within the same program unit must have a different
name and it must not be a subprogram or variable name.
Note that a REMOTE-block is local to the program unit in which it is
defined and may not be referenced (executed) from another program
unit.
.np
REMOTE-blocks may be defined anywhere in the program unit except as
follows.
.autopoint
.point
They must follow all specification statements.
.point
They must not be defined within a control structure.
.endpoint
.np
If a
.kw REMOTE BLOCK
statement is encountered during execution, control is transferred to
the statement following the corresponding
.kw END BLOCK
statement.
.sk 1 c
Note that the nested definition of REMOTE-blocks is not permitted.
.keep 20
.exam begin
      EXECUTE A
      PRINT *, 'FIRST'
          .
          .
          .
      EXECUTE A
      PRINT *, 'SECOND'
          .
          .
          .
      REMOTE BLOCK A
          I = I + 1
          PRINT *, 'I=', I
      END BLOCK
.exam end
.np
Both
.kw EXECUTE
statements will cause REMOTE-block
.id A
to be executed.
That is, variable
.id I
will be incremented and its value will be printed.
When the block has been executed by the first
.kw EXECUTE
statement, control returns to the
.kw PRINT
statement following it and the word
.mono FIRST
is printed.
Similarly, when the block is executed by the second
.kw EXECUTE
statement, control returns to the
.kw PRINT
statement following it and the word
.mono SECOND
is printed.
.np
REMOTE-blocks may be executed from other REMOTE-blocks.
For example, REMOTE-block
.id A
might contain the statement
.mono EXECUTE B,
where
.id B
is a REMOTE-block defined elsewhere in the program unit.
The execution of REMOTE-blocks from other REMOTE-blocks may take place
to any level; however, the recursive execution of REMOTE-blocks is not
permitted, either directly or through a chain of
.kw EXECUTE
statements.
Attempts to execute REMOTE-blocks recursively are detected as errors
at execution time.
.*
.keep 18
.section GUESS-ADMIT-END GUESS
.*
.mext begin
  GUESS    [: block-label]
      statement(s)
  ADMIT
      statement(s)
  ADMIT
      statement(s)
      .
      .
      .
  ADMIT
      statement(s)
  END GUESS
.mext end
.np
The GUESS-ADMIT-END GUESS structure is a rejection mechanism which is
useful when sets of statements are to be conditionally chosen for
execution, but not all of the conditions required to make a selection
are available beforehand.
It is an extension to FORTRAN 77.
The sets of statements to be chosen may be thought of as alternatives,
the first alternative being statements immediately after the
.kw GUESS
statement.
Execution begins with the statements in the first alternative.
If a condition is detected which indicates that the first alternative
was the wrong choice, a
.kw QUIT
statement may be executed to cause control to be passed to the
statements after the
.kw ADMIT
statement (i.e., the second alternative).
A
.kw QUIT
statement within the second alternative passes control to the third
alternative, etc.
A
.kw QUIT
statement within the last alternative passes control to the statement
after the
.kw END GUESS
statement.
If an alternative completes execution without encountering a
.kw QUIT
statement (i.e., all statements are executed up to the next
.kw ADMIT
statement) then control is passed to the statement after the
.kw END GUESS
statement.
An optional block label may be specified following the keyword
.kw GUESS
(see the
.kw QUIT
statement for more information).
.np
In the following example, two sets of codes and numbers are read in
and some simple sequence checking is performed.
If a sequence error is detected an error message is printed and
processing terminates; otherwise the numbers are processed and another
pair of numbers is read.
.keep 20
.exam begin
      LOOP : PRLOOP
          GUESS
              LINE = LINE + 1
              READ *, ICODE, X
              AT END, QUIT :PRLOOP
              IF( ICODE .NE. 1 )QUIT
              LINE = LINE + 1
              READ *, ICODE, Y
              AT END, QUIT
              IF( ICODE .NE. 2 )QUIT
              PRINT *, X, Y
              CALL PROCES(X, Y)
          ADMIT
              PRINT *, 'INVALID SEQUENCE: LINE =', LINE
              QUIT :PRLOOP
          END GUESS
      END LOOP
.exam end
.np
The above example attempts to read a code and number.
If an end of file occurs then the loop is terminated by the
.kw QUIT
statement.
.np
If the code is not 1 then we did not get what we expected and an
error situation has arisen.
Control is passed to the statement following the
.kw ADMIT
statement.
An error message is printed and the loop is terminated by the
.kw QUIT
statement.
.np
If the code is 1 then a second code and number are read.
If an end of file occurs then we are missing a set of data and an
error situation has arisen.
Control is passed to the statement following the
.kw ADMIT
statement.
An error message is printed and the loop is terminated by the
.kw QUIT
statement.
Similarly if the expected code is not 2 an error situation has arisen.
Control is passed to the statement following the
.kw ADMIT
statement.
An error message is printed and the loop is terminated by the
.kw QUIT
statement.
.np
If the second code is 2, the values of variables
.id X
and
.id Y
are printed.
A subroutine is then called to process the data.
Control resumes at the statement following the
.kw END GUESS
statement.
Since this statement is an
.kw END LOOP
.ct , control is transferred to the beginning of the loop.
.np
The above example illustrates the point that all the information
required to make a choice (in this case between a valid set of data
and an invalid set) is not available from the beginning.
In this case we make an assumption that the data values are correct
(our hypothesis) and then test the assumption at various points in the
algorithm.
If any of the tests fail we reject the hypothesis (and, perhaps,
select a new hypothesis).
.np
It should be noted that no alternative selection need be coded (i.e.,
we need not use any ADMIT-blocks).
This is illustrated in the following example.
.keep 10
.exam begin
      GUESS
          X=SQRT( X )
          IF( X .LT. EPS )QUIT
          X=Y+SQRT(Y)
          IF( X .LT. EPS )QUIT
          CALL INTGRT( X, Y )
      END GUESS
.exam end
.np
It might be noted that the IF-ELSE-END IF construct is simply
a specific instance of the more general GUESS-ADMIT-END GUESS
construct wherein the data values are known beforehand (as could be
illustrated using the previous example).
.*
.keep 10
.section QUIT
.*
.mext begin
  QUIT    [ : block-label]
.mext end
.np
The
.kw QUIT
statement may be used to transfer control to the first executable
statement following the terminal statement of the block in which it is
contained.
.np
When transferring out of a loop, control is passed to the statement
following the
.kw END DO
.ct ,
.kw END WHILE
.ct ,
.kw END LOOP
or
.kw UNTIL
statement.
.np
When transferring out of a GUESS block, control is passed to the
statement after the next
.kw ADMIT
or
.kw END GUESS
statement.
.np
When transferring out of an IF-block or SELECT-block,
control is passed to the statement after the corresponding
.kw END IF
or
.kw END SELECT
statement.
.np
When transferring out of a REMOTE-block, control passes to the
statement following the
.kw EXECUTE
statement that invoked the REMOTE-block.
.np
If no block label is specified in the
.kw QUIT
statement, control is transferred from the immediately enclosing
structure.
If several structures or DO-loops are nested, it is possible to exit
from any one of them by specifying the block label of the
corresponding block structure.
.np
The
.kw QUIT
statement is most commonly used as the statement in a logical
.kw IF
or
.kw AT END
statement but may also be used to cause an unconditional transfer of
control.
(The
.kw AT END
statement is described in a subsequent section).
.np
Examples of the
.kw QUIT
statement with and without a block label follow.
.keep 10
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
The above statements read and print values until an end of file
occurs.
At that point control is passed to the
.kw QUIT
statement, as specified by the
.kw AT END
statement.
The
.kw QUIT
statement causes control to continue with the statement after the
.kw END LOOP
statement.
.keep 15
.exam begin
      CHARACTER RECORD(80)
      LOOP : RDREC
          READ(5,100) RECORD
          AT END, STOP
          DO I = 1, 80
              IF( RECORD(I) .LT. '0'
     +              .OR. RECORD(I) .GT. '9' )QUIT : RDREC
          END DO
          WRITE(6,101) RECORD
      END LOOP
      PRINT *, 'INVALID RECORD'
.exam end
.np
The above example reads in records and verifies that they contain
only numeric data.
The
.kw QUIT
statement is within two levels of nesting: the DO-loop and the
LOOP-END LOOP structure.
If a non-numeric character is found, the
.mono QUIT : RDREC
statement will cause control to be passed to the
.kw PRINT
statement after the
.kw END LOOP
statement.
.*
.keep 10
.section EXIT
.*
.mext begin
  EXIT    [ : block-label]
.mext end
.np
The
.kw EXIT
statement is used to transfer control:
.autopoint
.point
from within a loop (DO, DO WHILE, WHILE or LOOP) to the
statement following the loop,
.point
from within a GUESS or ADMIT block to the statement following
the
.kw ENDGUESS
statement, or
.point
from within a remote block to the statement following the
.kw EXECUTE
statement that invoked the remote block.
.endpoint
.np
When transferring out of a loop, control is passed to the statement
following the
.kw END DO
.ct ,
.kw END WHILE
.ct ,
.kw END LOOP
or
.kw UNTIL
statement.
.np
When transferring out of a GUESS block, control is passed to the
statement after the corresponding
.kw END GUESS
statement.
.np
When transferring out of a REMOTE-block, control passes to the
statement following the
.kw EXECUTE
statement that invoked the REMOTE-block.
.np
If no block label is specified in the
.kw EXIT
statement, control is transferred from the immediately enclosing
structure.
If several structures or DO-loops are nested, it is possible to exit
from any one of them by specifying the block label of the
corresponding block structure.
.np
The
.kw EXIT
statement is most commonly used as the statement in a logical
.kw IF
or
.kw AT END
statement but may also be used to cause an unconditional transfer of
control.
(The
.kw AT END
statement is described in a subsequent section).
.np
Examples of the
.kw EXIT
statement with and without a block label follow.
.keep 10
.exam begin
      LOOP
          READ *, X
          AT END, EXIT
          PRINT *, X
      END LOOP
.exam end
.np
The above statements read and print values until an end of file
occurs.
At that point control is passed to the
.kw EXIT
statement, as specified by the
.kw AT END
statement.
The
.kw EXIT
statement causes control to continue with the statement after the
.kw END LOOP
statement.
.keep 15
.exam begin
      CHARACTER RECORD(80)
      LOOP : RDREC
          READ(5,100) RECORD
          AT END, STOP
          DO I = 1, 80
              IF( RECORD(I) .LT. '0'
     +              .OR. RECORD(I) .GT. '9' )EXIT : RDREC
          END DO
          WRITE(6,101) RECORD
      END LOOP
      PRINT *, 'INVALID RECORD'
.exam end
.np
The above example reads in records and verifies that they contain
only numeric data.
The
.kw EXIT
statement is within two levels of nesting: the DO-loop and the
LOOP-END LOOP structure.
If a non-numeric character is found, the
.mono EXIT : RDREC
statement will cause control to be passed to the
.kw PRINT
statement after the
.kw END LOOP
statement.
.*
.keep 10
.section CYCLE
.*
.mext begin
  CYCLE   [ : block-label]
.mext end
.np
The
.kw CYCLE
statement is used to cause a transfer of control from within a loop to
the terminal statement of a corresponding
.kw DO
.ct ,
.kw DO WHILE
.ct ,
.kw WHILE
or
.kw LOOP
statement.
If
.id block-label
is present then control is transferred to the terminal statement of
the block identified by that block label.
.np
If no block label is specified in the
.kw CYCLE
statement, control is transferred to the terminal statement of the
immediately enclosing loop structure.
If several loop structures are nested, it is possible to cycle to the
terminal statement of any one of them by specifying the block label of
the corresponding block structure.
.np
The
.kw CYCLE
statement is most commonly used as the statement in a logical
.kw IF
statement but may also be used to cause an unconditional transfer of
control.
.np
Examples of the
.kw CYCLE
statement with and without a block label follow.
.keep 10
.exam begin
      LOOP
          WRITE( UNIT=*, FMT='(A)' ) 'Enter a number'
          READ( UNIT=*, FMT='(F10.4)', IOSTAT=IOS ) X
          IF( IOS .NE. 0 ) CYCLE
          IF( X .LT. 0 ) EXIT
          PRINT *, X, SQRT( X )
      END LOOP
.exam end
.np
The above statements read and print values until a negative integer
value is entered.
If an input error occurs, the input operation (READ) is retried using
the
.kw CYCLE
statement.
The
.kw CYCLE
statement causes control to resume at the
.kw END LOOP
statement which then immediately transfers control to the statement
following the
.kw LOOP
statement.
.keep 15
.exam begin
      CHARACTER RECORD(80)
      LOOP : RDREC
          READ(5,100) RECORD
          AT END, STOP
          DO I = 1, 80
              IF( RECORD(I) .LT. '0'
     +              .OR. RECORD(I) .GT. '9' )THEN
                  PRINT *, 'INVALID RECORD'
                  CYCLE : RDREC
              ENDIF
          END DO
          WRITE(6,101) RECORD
      END LOOP
.exam end
.np
The above example reads in records and verifies that they contain only
numeric data.
If the record does not, the input operation is tried again.
The
.kw CYCLE
statement is within three levels of nesting: the IF, the DO-loop, and
the LOOP-END LOOP structure.
If a non-numeric character is found, the
.mono CYCLE : RDREC
statement will cause control to be passed to the
.kw READ
statement that follows the
.kw LOOP
statement.
.*
.keep 10
.section AT END
.*
.mext begin
  (READ statement)
  AT END DO [: block-label ]
      statement(s)
  END AT END
.mext end
.pc
or
.mext begin
  (READ statement)
  AT END, statement
.mext end
.np
The
.kw AT END
control statement, an extension to FORTRAN 77, is an extension of the
.kw END=
option of the FORTRAN
.kw READ
statement for sequential files.
It allows a statement or a block of code following the
.kw READ
statement to be executed when an end of file condition is encountered
during the
.kw READ
and to be by-passed immediately following a
.kw READ
statement.
It is not valid to use this control statement with direct-access or
memory-to-memory reads.
Clearly, it is not valid to use this statement when
.kw END=
is specified in the
.kw READ
statement.
.keep 10
.exam begin
      READ(7, *) I, X
      AT END DO
          PRINT *, 'END-OF-FILE ENCOUNTERED'
          EOFSW = .TRUE.
      END AT END
.exam end
.np
If the
.kw READ
statement is executed without encountering end of file, control passes
to the statement following the
.kw END AT END
statement.
If an end of file condition occurs during the read, the string,
.mono END-OF-FILE ENCOUNTERED
is printed, logical variable
.id EOFSW
is assigned the value
.mono .TRUE.
and control passes to the statement following the
.kw END AT END
statement.
.keep 10
.exam begin
      READ(7, *) X
      AT END, EOFSW = .TRUE.
.exam end
.np
If an end of file is not encountered by the
.kw READ
statement, control passes to the statement following the
.kw AT END
statement.
If an end-of-file condition occurs, variable
.id EOFSW
is set to
.mono .TRUE.
and control then passes to the statement following the
.kw AT END
statement.
Note that the use of the second form of the
.kw AT END
statement requires the use of a comma (,) between the
.mono AT END
word and the executable statement.
This is necessary to distinguish the case where the executable
statement is an assignment statement.
The executable statement may be any statement that is also allowed as
the operand of a logical
.kw IF
statement.
.*
.section Notes on Structured Programming Statements
.*
.np
In addition to the definitions and examples of these constructs, the
following points should be noted:
.begpoint
.point (i)
Any of the new control statements with their blocks may be used within
the block of any other statement.
For example, a WHILE-block may contain another WHILE or an
IF-THEN-ELSE block.
Blocks may be nested in this manner to any level within storage
limitations.
An important exception to this rule is the REMOTE-block A REMOTE-block
may contain other types of blocks (nested to any level); however,
another REMOTE-block may not be defined within it.
Furthermore, REMOTE-blocks may not be defined within another control
structure.
The following example is illegal.
.keep 10
.exam begin
* Illegal definition of a REMOTE-block.
      IF( I .EQ. 3 )then
          REMOTE BLOCK A
              .
              .
              .
          END BLOCK
      END IF
.exam end
.point (ii)
When nesting blocks, the inner blocks must always be completed with
the appropriate block-terminating
.kw END
statement before the outer blocks are terminated.
Similarly, when nesting blocks with DO-loops, a DO-loop started within
a block must be completed before the block is completed.
A block started within a DO-loop must be terminated before the DO-loop
is completed.
Indenting the statements of each new block, as shown in the examples,
is helpful in avoiding invalid nesting and helps to make the structure
of the program visually obvious.
.point (iii)
The normal flow of control of the new programming constructs described
earlier may be altered with standard FORTRAN control statements.
For example, the program may exit from a block using a
.kw GO TO
.ct ,
.kw STOP
.ct ,
.kw RETURN
or arithmetic
.kw IF
statement.
However, a block may not be entered in the middle through use of any
control statement such as
.kw GO TO
or the arithmetic
.kw IF
.ct .li .
.np
Consider the following example.
.keep 10
.exam begin
      GO TO 20
10    IF( X .GT. Y )THEN
          CALL REDUCE( X, Y )
20        X = X - 1
      ELSE
          CALL SCALE( X )
      END IF
.exam end
.pc
This is an example of an illegal attempt to transfer execution into
the middle of an IF-block.
The statement
.mono X = X - 1
is contained within the IF-block and may only be transferred to from
within the block.
.keep 10
.exam begin
      IF( X .GT. Y )THEN
20        CALL REDUCE( X, Y )
          X = X - 1
          IF( X .GT. 0 )GO TO 20
      ELSE
          CALL SCALE( X )
      END IF
.exam end
.pc
This last example demonstrates a legal transfer of control within an
IF-block.
However, we have seen better ways to express the loop with this
IF-block.
.keep 10
.exam begin
      IF( X .GT. Y )THEN
          LOOP
              CALL REDUCE( X, Y )
              X = X - 1
          UNTIL( X .LE. 0 )
      ELSE
          CALL SCALE( X )
      END IF
.exam end
.point (iv)
Many control structure statements cannot be branched to using a
.kw GO TO
statement.
For a list of these statements,
see the section entitled :HDREF refid='stmtcls'. in the chapter
entitled :HDREF refid='fstats' page=no..
.point (v)
Many control structure statements cannot be the object statement of a
logical
.kw IF
statement, or be the last statement of a DO-loop.
For a list of these statements,
see the section entitled :HDREF refid='stmtcls'. in the chapter
entitled :HDREF refid='fstats' page=no..
.endpoint
