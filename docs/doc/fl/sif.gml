.section Arithmetic IF Statement
.*
.mbox begin
      IF (e) s1, s2, s3
.mbox end
.synote 14
.mnote e
is an integer, real, or double precision expression.
.mnote s1, s2, s3
are statement labels of executable statements that appear
in the same program unit as the arithmetic
.kw IF
statement.
.endnote
.np
The expression
.id e
is evaluated and if the value is less than zero
then transfer is made to the statement identified by label
.id s1.
If the value is equal to zero then transfer is made to the
statement identified by label
.id s2.
If the value is greater than zero then transfer is made to
the statement identified by label
.id s3.
.exam begin
      IF( SIN( X ) ) 10, 20, 30
10    PRINT *, 'SIN(X) IS < 0'
      GO TO 40
20    PRINT *, 'SIN(X) = 0'
      GO TO 40
30    PRINT *, 'SIN(X) > 0'
40    CONTINUE
.exam end
.pc
The above example evaluates the sine of the real variable
.id X
and prints whether the result is less than 0, equal to 0, or greater
than 0.
.np
The same label may appear more than once in the arithmetic
.kw IF
statement.
.exam begin
      IF( SIN( X ) ) 10, 10, 30
10    PRINT *, 'SIN(X) IS <= 0'
      GO TO 40
30    PRINT *, 'SIN(X) > 0'
40    CONTINUE
.exam end
.pc
The above example evaluates the sine of the real variable
.id X
and prints whether the result is less than or equal to zero, or that
it is greater than 0.
.*
.stmt IF (Logical)
.*
.section Logical IF Statement
.*
.mbox begin
      IF (e) st
.mbox end
.synote
.mnote e
is a logical expression
.xt on
or integer arithmetic expression, in which case the result of the
integer expression is compared for inequality to the integer value 0.
.xt off
.mnote st
is an executable statement.
Only certain executable statements are allowed.
See the section entitled :HDREF refid='stmtcls'. at the beginning of
this chapter for a list of allowable statements.
.endnote
.np
The expression
.id e
is evaluated and must result in a true or a false value.
If the result is true then the statement
.id st
is executed, otherwise it is not executed.
.exam begin
      IF( A .LT. B )PRINT *, 'A < B'
.exam end
.pc
In the above example, the logical expression
.mono A .LT. B
is evaluated and, if it is true, the message
.mono A < B
is printed.
A logical expression is one in which the result is either true or
false.
An expression such as
.mono 1 + 2
is clearly not an example of a logical expression.
.np
Logical variables have logical values of true or false and
may also be used in the logical expression.
Consider the following two examples.
.exam begin
      LOGICAL RESULT
      RESULT = A .LT. B
      IF( RESULT )PRINT *, 'A < B'
.exam end
.pc
The above example is equivalent to the
preceding one but introduces the use of a logical variable.
.exam begin
      LOGICAL RESULT
      RESULT = A .LT. B
      IF( .NOT. RESULT )PRINT *, 'A >= B'
.exam end
.pc
In the above example, the logical expression is negated through
the use of the
.mono .NOT.
operator in order to test for the inverse condition, namely
.mono .GE..
.np
Much more complex logical expressions can be constructed
and then tested for their truth value.
.exam begin
      IF( A.LT.B .OR. C.GE.D )PRINT *, 'A<B or C>=D'
.exam end
.np
.cp 12
.xt begin
An example of an integer expression in an
.kw IF
statement follows:
.exam begin
      I = 1
* Integer arithmetic expression
      IF( I )THEN
              PRINT *, 'Yes'
      ENDIF
* Equivalent logical expression
      IF( I .NE. 0 )THEN
              PRINT *, 'Yes'
      ENDIF
      END
.exam end
.xt end
.*
.stmt IF (Block)
.*
.section Block IF Statement
.*
.np
There are two forms of the block
.kw IF
statement.
The second is a &product extension.
.beglevel
.*
.section Standard Block IF Statement
.*
.mbox begin
      IF (e) THEN
.mbox end
.synote
.mnote e
is a logical expression.
.endnote
.np
The block
.kw IF
statement is used in conjunction with the
.kw ELSE IF
.ct ,
.kw ELSE
.ct , and
.kw END IF
statements.
.exam begin
      IF( A .LT. B )THEN
          PRINT *, 'A < B'
      END IF
.exam end
.np
For more information, see the chapter entitled :HDREF refid='fstruct'..
.*
.section Extended Block IF Statement
.*
.mext begin
      IF (e) THEN    [: block-label]
.mext end
.synote 15
.mnote e
is a logical expression or integer arithmetic expression, in which
case the result of the integer expression is compared for inequality
to the integer value 0.
.mnote block-label
is an optional block label.
.endnote
.np
This form of the block
.kw IF
statement is an extension to the FORTRAN 77 language.
It is identical to the standard form of the block
.kw IF
statement with the exception that an integer arithmetic expression and
an optional block label are permitted.
.exam begin
      IF( I .EQ. 10 )THEN : IFBLK
          IF( J .EQ. 20 )THEN
              .
              .
              .
              IF( K. EQ. 0 )QUIT : IFBLK
              .
              .
              .
          END IF
          .
          .
          .
      END IF
.exam end
.pc
In the previous example, the
.kw QUIT
statement will transfer control to the statement following the second
.kw END IF
statement.
.endlevel
