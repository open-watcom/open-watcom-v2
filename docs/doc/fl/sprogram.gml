.section PROGRAM Statement
.*
.mbox begin
      PROGRAM pgm
.mbox end
.synote 5
.mnote pgm
is the symbolic name of the main program.
.endnote
.np
A
.kw PROGRAM
statement is optional in an executable program.
If it does appear, it must be the first statement in the main program.
.exam begin
      PROGRAM CALC
          .
          .
          .
      CALL COMPUTE
          .
          .
          .
      END
.exam end
.np
The main program can contain any &product statement except a
.kw FUNCTION
.ct ,
.kw SUBROUTINE
.ct ,
.kw BLOCK DATA
.ct ,
.kw RETURN
or
.kw ENTRY
statement.
Note that a
.kw SAVE
statement is allowed but has no effect in the main program.
