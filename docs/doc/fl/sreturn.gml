.section RETURN Statement
.*
.mbox begin
      RETURN [e]
.mbox end
.synote
.mnote e
is an integer expression.
.endnote
.np
A
.kw RETURN
statement is used to terminate execution of a subprogram and return
control to the program unit that referenced it.
.xt on
As an extension to FORTRAN 77, &product permits the use of the
.kw RETURN
statement in the main program.
When a
.kw RETURN
statement is executed in the main program, program execution
terminates in the same manner as the
.kw STOP
or
.kw END
statement.
.xt off
.np
The expression
.id e
is not permitted when returning from an external function subprogram
(or main program);
it can only be specified when returning from a subroutine subprogram.
.exam begin
      FUNCTION ABS( A )
      ABS = A
      IF( A .GE. 0 )RETURN
      ABS = -A
      RETURN
      END
.exam end
.np
For more information, see the chapter entitled :HDREF refid='fsubp'..
