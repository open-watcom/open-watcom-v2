.section REMOTE BLOCK Statement
.*
.mext begin
      REMOTE BLOCK name
.mext end
.synote 6
.mnote name
is a valid FORTRAN symbolic name.
.endnote
.np
The
.kw REMOTE BLOCK
statement is used to define a block of statements which may be
executed by an
.kw EXECUTE
statement.
A REMOTE-block must be defined in the program unit in which it is used
and is terminated by an
.kw END BLOCK
statement.
A REMOTE-block is similar in concept to a subroutine, with the
advantage that shared variables do not need to be placed in a
common block or passed in an argument list.
When execution of the REMOTE-block is complete, control returns to the
statement following the
.kw EXECUTE
statement which invoked it.
.np
This feature is helpful in avoiding duplication of code for a common
sequence of statements required in a number of places throughout a
program.
It can also be an aid to writing a well structured program.
This feature can be mimicked using the
.kw ASSIGN
and assigned
.kw GO TO
statements.
However, statement numbers must be introduced which could lead to
errors.
.np
Each REMOTE-block must have a different name and it must not be a
subprogram or variable name.
Note that a REMOTE-block is local to the program unit in which it is
defined and may not be referenced (executed) from another program
unit.
.np
Note that the nested definition of REMOTE-blocks is not permitted.
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
Both
.kw EXECUTE
statements will cause REMOTE-block
.id INCR
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
For more information, see the chapter entitled :HDREF refid='fstruct'..
