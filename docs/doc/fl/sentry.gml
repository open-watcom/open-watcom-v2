.section ENTRY Statement
.*
.mbox begin
      ENTRY name [( [d [, d] ...] )]
.mbox end
.synote 6
.mnote name
is a symbolic name of an entry in a function or subroutine subprogram.
If the
.kw ENTRY
statement appears in a subroutine subprogram then
.id name
is a
.us subroutine name.
.ix 'subroutine' 'name'
If the
.kw ENTRY
statement appears in a function subprogram then
.id name
is an
.us external function name.
.ix 'external function name'
.ix 'function' 'external name'
.mnote d
is a variable name, array name, dummy procedure name, or an asterisk.
.id d
is called a
.us dummy argument.
.ix 'dummy argument'
.ix 'argument' 'dummy'
An asterisk is allowed only in a subroutine subprogram.
.endnote
.np
The
.kw ENTRY
statement is used to define an alternate entry into a subprogram.
.exam begin
      PRINT *, TMAX2( 121.0, -290.0 )
      PRINT *, TMAX3( -1.0, 12.0, 5.0 )
      END

      FUNCTION TMAX3( ARGA, ARGB, ARGC )
          T3 = ARGC
          GO TO 10
      ENTRY TMAX2( ARGA, ARGB )
          T3 = ARGA
10        TMAX2 = ARGA
          IF( ARGB .GT. TMAX2 ) TMAX2 = ARGB
          IF( T3 .GT. TMAX2 ) TMAX2 = T3
      END
.exam end
.pc
In the above example, an entry was defined to permit us to find the
maximum of two real variables.
Either the entry name
.id TMAX2
or the function name
.id TMAX3
could have been used as the variable for returning the maximum value
since they agree in type.
It is not necessary to precede an
.kw ENTRY
statement with a transfer statement as the
.kw ENTRY
statement is not an executable statement; the next statement executed
will be the first executable statement following the
.kw ENTRY
statement.
.autonote Notes:
.setptnt 0 5
.note
No dummy arguments need be specified in the
.kw ENTRY
statement.
If this is the case, the parentheses () are optional.
.endnote
.np
For more information, see the chapter entitled :HDREF refid='fsubp'..
