.section END BLOCK Statement
.*
.mext begin
      END BLOCK
.mext end
.np
The
.kw END BLOCK
statement is used to terminate a REMOTE-block.
The
.kw END BLOCK
statement is implicitly a transfer statement, since it returns
program control from a REMOTE-block.
.exam begin
      REMOTE BLOCK A
          I=I+1
          PRINT *, 'I=',I
      END BLOCK
.exam end
.np
For more information, see the description of the
.kw EXECUTE
and
.kw REMOTE BLOCK
statements or the chapter entitled :HDREF refid='fstruct'..
