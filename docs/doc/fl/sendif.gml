.section END IF Statement
.*
.mbox begin
      END IF
.mbox end
.np
The
.kw END IF
statement is used in conjunction with the block
.kw IF
statement.
The
.kw END IF
statement marks the end of a sequence of statements which are to be
conditionally executed.
.exam begin
      IF( X .LT. 100.0 )THEN
          PRINT *, 'X IS LESS THAN 100'
      END IF
.exam end
.pc
The
.kw END IF
statement can also be used in conjunction with the
.kw ELSE
and
.kw ELSE IF
statements.
For more information, see the chapter entitled :HDREF refid='fstruct'..
