.section EXECUTE Statement
.*
.mext begin
      EXECUTE name
.mext end
.synote 6
.mnote name
is the name of a
.kw REMOTE BLOCK
located in the same program unit.
.endnote
.np
The
.kw EXECUTE
statement allows a named block of code to be executed.
The named block of code may be defined anywhere in the same program
unit and is delimited by the
.kw REMOTE BLOCK
and
.kw END BLOCK
statements.
Executing a REMOTE-block is similar in concept to calling a
subroutine, with the advantage that shared variables do not need to be
placed in a
.kw COMMON
block or passed in an argument list.
When execution of the REMOTE-block is complete (i.e., when the
.kw END BLOCK
statement is executed), control returns to the statement following the
.kw EXECUTE
statement which invoked it.
.exam begin
      EXECUTE INCR
      PRINT *, 'FIRST'
      EXECUTE INCR
      PRINT *, 'SECOND'
          .
          .
          .
      REMOTE BLOCK INCR
          I=I+1
          PRINT *, 'I=',I
      END BLOCK
.exam end
.np
For more information, see the chapter entitled :HDREF refid='fstruct'..
