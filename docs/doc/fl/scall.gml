.section CALL Statement
.*
.mbox begin
      CALL sub [( [a [, a] ... ] )]
.mbox end
.synote 5
.mnote sub
is a symbolic name of a subroutine and must not be the name of a main
program, function, common block, or block data subprogram.
.xt on
As an extension to FORTRAN 77, &product permits the calling of
functions.
.xt off
.mnote a
is an actual argument.
.endnote
.np
The
.kw CALL
statement is used to invoke the execution of a subroutine subprogram
.xt or function.
.exam begin
      X = 1.0
      Y = 1.1
      Z = 1.2
      CALL QUAD( X, Y, Z )
          .
          .
          .
      END

      SUBROUTINE QUAD( ARGA, ARGB, ARGC )
      REAL ARGA, ARGB, ARGC
      PRINT *, 2.0*ARGA**2 + 4.0*ARGB + ARGC
      END
.exam end
.pc
In the above example, the variables
.id X,
.id Y
and
.id Z
are passed to the subroutine
.id QUAD.
This subroutine computes an expression and prints the result.
.autonote Notes:
.setptnt 0 5
.note
The parameters in the
.kw CALL
statement are called
.us actual arguments.
.ix 'actual argument'
.note
The parameters in the
.kw SUBROUTINE
statement are called
.us dummy arguments.
.ix 'dummy argument'
.note
The actual arguments in a subroutine
.xt or function
reference must agree in order, number and type with the corresponding
dummy arguments.
.note
An actual argument may be an expression, array name, intrinsic function
name, external procedure name (i.e., a subroutine or function name), a
dummy procedure name (i.e., one that was an argument to the calling
subroutine or function), or an
.us alternate return specifier
(subroutines only).
.ix 'alternate return specifier'
An alternate return specifier takes the form
.id *s,
where
.id s
is the statement label of an executable statement that appears in the
same program unit as the
.kw CALL
statement.
An expression may not be a character expression involving the
concatenation of an operand whose length specification is
.mono (*)
unless the operand is the symbolic name of a constant.
.note
Actual arguments are associated with dummy arguments by passing the
address of the actual arguments.
.np
It is important to note that versions of FORTRAN compilers that
implement the previous FORTRAN language standard may have
associated arguments by passing the value of the actual argument
and assigning it to the dummy argument and then updating the
actual argument upon return from the subprogram (this is called
"value-result" argument handling).
The FORTRAN 77 language standard prohibits this technique for handling
argument association.
.np
The following example illustrates the importance of this rule.
.exam begin
      I=1
      CALL ASSOC( I, I )
      END
      SUBROUTINE ASSOC( M, N )
      M = M + 1
      PRINT *, M, N
      END
.exam end
.pc
In the above example,
.id M
and
.id N
refer to the same variable; they are both associated to
.id I
in the calling subprogram.
The value 2 will be printed twice.
.endnote
.np
For more information, see the chapter entitled :HDREF refid='fsubp'..
