.section ELSE Statement
.*
.mbox begin
      ELSE
.mbox end
.np
The
.kw ELSE
statement is used in conjunction with the
.kw IF
or
.kw ELSE IF
statement.
The range of the
.kw ELSE
block is terminated by a matching
.kw END IF
statement.
.exam begin
      IF( A .LT. B )THEN
          PRINT *, 'A is less than B'
      ELSE
          PRINT *, 'A is greater than or equal to B'
      END IF
.exam end
.np
Transfer of control into the range of an
.kw ELSE
block is illegal.
It is interesting to note that the
.kw ELSE
statement may be identified by a statement label but it must not
be referenced by any statement!
.exam begin
* Illegal branch to a labelled ELSE statement.
      IF( A .LT. B )THEN
          PRINT *, 'A is less than B'
100   ELSE
          PRINT *, 'A is greater than or equal to B'
          GO TO 100
      END IF
.exam end
.pc
The above is an example of an illegal way to construct an
infinitely repeating loop.
The following is the correct way to do this.
.exam begin
      IF( A .LT. B )THEN
          PRINT *, 'A is less than B'
      ELSE
100       PRINT *, 'A is greater than or equal to B'
          GO TO 100
      END IF
.exam end
.np
For more information, see the chapter entitled :HDREF refid='fstruct'..
