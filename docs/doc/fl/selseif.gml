.section ELSE IF Statement
.*
.mbox begin
      ELSE IF (e) THEN
.mbox end
.synote
.mnote e
is a logical expression
.xt on
or integer arithmetic expression, in which case the result of the
integer expression is compared for inequality to the integer value 0.
.xt off
.endnote
.np
The
.kw ELSE IF
statement is used in conjunction with the
.kw IF
statement.
The range of the
.kw ELSE IF
block is terminated by another
.kw ELSE IF
statement, an
.kw ELSE
statement, or an
.kw END IF
statement.
.exam begin
      IF( A .LT. B )THEN
          PRINT *, 'A is less than B'
      ELSE IF( A .EQ. B )THEN
          PRINT *, 'A is equal to B'
      ELSE
          PRINT *, 'A is greater than B'
      END IF
.exam end
.np
Transfer of control into the range of an
.kw ELSE IF
block is illegal.
It is interesting to note that the
.kw ELSE IF
statement may be identified by a statement label but it must not
be referenced by any statement!
.exam begin
* Illegal transfer into the range of
* an ELSE IF statement.
      IF( A .EQ. 0.0 )GO TO 110
      IF( A .LT. B )THEN
          PRINT *, 'A is less than B'
      ELSE IF( A .EQ. B )THEN
          PRINT *, 'A is equal to B or'
110       PRINT *, 'A is equal to 0'
      ELSE
          PRINT *, 'A is greater than B'
      END IF
.exam end
.pc
The above is an example of an illegal attempt to branch into
the range of an
.kw ELSE IF
block.
.np
For more information, see the chapter entitled :HDREF refid='fstruct'..
