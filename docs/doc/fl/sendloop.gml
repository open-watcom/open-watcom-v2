.section END LOOP Statement
.*
.mext begin
      END LOOP
.mext end
.np
The
.kw END LOOP
statement is used in conjunction with the structured
.kw LOOP
statement.
The
.kw END LOOP
statement marks the end of a sequence of statements which are to be
repeated.
The
.kw LOOP
statement marks the beginning of the loop.
The LOOP-block is executed until control is transferred out of the
LOOP-block.
.np
The
.kw QUIT
statement may be used to transfer control out of a LOOP-block.
.exam begin
      LOOP
          READ *, X
          IF( X .GT. 99.0 ) QUIT
          PRINT *, X
      END LOOP
.exam end
.np
For more information, see the chapter entitled :HDREF refid='fstruct'..
