.section SUBROUTINE Statement
.*
.mbox begin
      SUBROUTINE sub [( [d [, d] ...] )]
.mbox end
.synote 5
.mnote sub
is a symbolic name of a subroutine subprogram.
.mnote d
is a variable name, array name, dummy procedure name or an asterisk
(*).
.id d
is called a
.us dummy argument.
.ix 'dummy argument'
.ix 'argument' 'dummy'
.endnote
.np
A
.kw SUBROUTINE
statement is used to define the start of a subroutine subprogram.
.exam begin
      CALL TMAX3( -1.0, 12.0, 5.0 )
      END

      SUBROUTINE TMAX3( ARGA, ARGB, ARGC )
          THEMAX = ARGA
          IF( ARGB .GT. THEMAX ) THEMAX = ARGB
          IF( ARGC .GT. THEMAX ) THEMAX = ARGC
          PRINT *, THEMAX
      END
.exam end
.pc
In the above example, the subroutine
.id TMAX3
is defined to find and print out the maximum value of three real
variables.
.autonote Notes:
.setptnt 0 5
.note
No dummy arguments need be specified in the
.kw SUBROUTINE
statement.
If such is the case, the parentheses () are optional.
.endnote
.np
For more information, see the chapter entitled :HDREF refid='fsubp'..
