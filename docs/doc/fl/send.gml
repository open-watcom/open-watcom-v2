.section END Statement
.*
.mbox begin
      END
.mbox end
.np
The
.kw END
statement indicates the end of a sequence of statements and
comment lines of a program unit.
Execution of an
.kw END
statement in a function or subroutine subprogram has the same
effect as a
.kw RETURN
statement.
Control is returned to the invoking program unit.
Execution of an
.kw END
statement in a main program causes termination of execution of
the program.
.exam begin
      SUBROUTINE EULER( X, Y, Z )
          .
          .
          .
      END
.exam end
.pc
Upon executing the
.kw END
statement, execution control is returned to the calling program
unit.
.exam begin
      PROGRAM PAYROL
          .
          .
          .
      END
.exam end
.pc
Upon executing the
.kw END
statement, execution of the program is terminated.
.np
Some rather special rules apply to the
.kw END
statement.
The statement is written in columns 7 to 72 of an initial line.
In other words, it must not be continued.
Also, no other statement in the program unit may have an initial line
that appears to be an
.kw END
statement.
.exam begin
* An illegal ENDIF statement.
      IF( A .LT. B )THEN
          .
          .
          .
      END
     &IF
.exam end
.pc
The above
.kw END IF
statement is illegal since the initial line appears to be an
.kw END
statement.
