.chap Differences Between WATFIV and &product
.*
.np
There are a number of differences between the WATFIV and WATFOR-11
compilers and the &cmpname compiler.
Many result from differences between the language set supported
by WATFIV/WATFOR-11 and the language set described by the ANSI
FORTRAN 77 standard.
WATFIV is a FORTRAN compiler for the IBM 370 series of computers.
WATFOR-11 is a FORTRAN compiler for the Digital PDP11 series
of minicomputers.
WATFIV and WATFOR-11 support a dialect of FORTRAN commonly known
as FORTRAN IV.
.np
&cmpname supports many WATFIV/WATFOR-11 features as extensions
to the FORTRAN 77 language.
The following is a list of some of the differences that we have
noted between the WATFIV/WATFOR-11 and &cmpname compilers.
.autonote
.note
WATFIV/WATFOR-11 are batch oriented, whereas &cmpname is not.
The source input file to WATFIV/WATFOR-11 could contain several
FORTRAN programs to be run.
These programs were separated from each other using "$JOB"
compiler directives.
The source input file to &cmpname can consist of only one program.
.note
Options of the form "$keyword" are not supported.
They must begin with a comment specifier (C, c, or *).
Thus, $LIST is illegal but C$LIST or *$LIST are acceptable.
In particular, the "$JOB" statement is not recognized, however
"C$JOB" will be treated as a comment.
.note
When using &cmpname, units 5 and 6 are the console (terminal) input
and output devices by default.
When using WATFIV/WATFOR-11, units 5 and 6 are the source input stream
and source listing file, respectively.
&cmpname allows you to create this environment using the C$DATA
directive and the "NOXTYPE" option.
.note
Not all of the WATFIV/WATFOR-11 compiler options are supported.
The ones that are supported are described in the chapter entitled
"Compiler Options".
.note
WATFIV supports compressed source programs using the ";" as a
statement separator.
&cmpname does not support this feature.
.exam begin
      I=1;X=2.0;CALL SUBR(I,X)
.exam end
.pc
Since &cmpname supports
..if '&machsys' = 'VM/SP CMS' | '&machsys' = 'VAX/VMS' ..do begin
object files as well as
..do end
variable length FORTRAN statements, it was felt that this feature
need not be supported.
.note
&cmpname does not support the FORTRAN 66 DO statement, in which
a DO-loop is always executed at least once, regardless of the
initial, terminal, and increment values.
In the following example, the statements inside the DO-loop will
never be executed.
If this same example was processed by WATFIV or WATFOR-11, the
statements inside the DO-loop would be executed once.
.exam begin
      DO 10 I=5,4,1
      .
      .
      .
10    CONTINUE
.exam end
.note
Many of the WATFIV/WATFOR-11 structured programming statements are
supported.
However, unlike WATFIV/WATFOR-11, &cmpname does not allow branches
into structure blocks (unless the "WILD" option is used).
This is in keeping with the FORTRAN 77 standard rules regarding
the block-IF.
.note
The DO CASE statement is not supported but a somewhat more modern
construct, the "SELECT" statement is supported.
The "SELECT" statement follows closely the FORTRAN 8x proposal, a
new FORTRAN language standard that is currently under
development.
.note
The "THEN DO" form of the block-IF statement is not supported.
FORTRAN 77 specifies that the word "THEN" must be used.
.note
Structure block "labels" are supported.
The form of the QUIT statement is different from WATFIV/WATFOR-11
which allowed specification of the number of levels to quit.
&cmpname accepts a block label as the operand of the QUIT statement.
.note
WATFIV/WATFOR-11's pseudo-variable dimensioning is supported as
an extension (the authors of
.us Structured FORTRAN with WATFIV-S
refer to this feature as "crypto-variable dimensioning").
If the last dimension in a dimension list of a dummy argument
array was 1 then WATFIV/WATFOR-11 allowed the array to assume the
largest dimensions possible within the limits of the actual
array.
This feature is used in many existing scientific subprogram
libraries.
FORTRAN 77 supports a feature called "assumed-size arrays" in
which the last dimension is specified as "*".
We recommend that you use the FORTRAN 77 method for conformance
with the standard.
.exam begin
      SUBROUTINE XDOTY( SX, TX  )
* WATFIV PSEUDO-VARIABLE DIMENSIONING
      REAL SX(1)
* FORTRAN 77 ASSUMED-SIZE ARRAY
      REAL TX(*)
      .
      .
      .
      END
.exam end
.pc
To avoid the assumption that the array
.mono SX
is an assumed-size array, you may declare both the lower and
upper bounds of the array as follows:
.exam begin
      REAL SX(1:1)
.exam end
.note
The handling of blanks in numeric input with WATFIV/WATFOR-11 is
equivalent to the BLANK='ZERO' specifier (see OPEN statement).
With &cmpname, the default is BLANK='NULL' (in accordance with the
FORTRAN 77 standard).
To illustrate the difference, consider what happens when the
numeric input "1&SYSRB.2&SYSRB.3" is read using the format
specification "(I5)".
With WATFIV, the resulting value would be 10203 whereas, with &cmpname
the resulting value would be 123 (unless BLANK='ZERO' was specified).
.note
WATFIV/WATFOR-11 allows specification statements to follow an
ENTRY statement provided the statement before the ENTRY was an
unconditional transfer.
This is not supported by &cmpname..
.exam begin
      FUNCTION SAM
      SAM = 1
      RETURN
      ENTRY SAM2
      DIMENSION A(2)
.exam end
.note
The PUNCH statement is allowed in WATFIV and WATFOR-11.
This statement is used to produce punched cards.
The statement may use list-directed formatting or a format
statement.
The record length must be 80.
&cmpname does not support the PUNCH statement.
.exam begin
      A = 10.0
      B = 33.33
      PUNCH,A,B
      END
.exam end
.note
&cmpname supports the FORTRAN 77 "*" operator for alternate returns.
WATFIV/WATFOR-11 use the "&" delimiter to signify this.
The error message that is issued is "invalid character encountered in
source input" since "&" is not in the FORTRAN character set.
.exam begin
      CALL READ(&999,&400,&300,&100)
.exam end
.pc
should be coded as
.exam begin
      CALL READ(*999,*400,*300,*100)
.exam end
.note
WATFIV/WATFOR-11 allow a hollerith constant to be passed as an
argument to a variable of any type.
&cmpname allows hollerith constants to be passed as an argument only
to a variable whose type is character.
.note
WATFIV/WATFOR-11 support passing of arguments by "value result"
or by "address".
&cmpname does not support the syntax of passing arguments by address
using the "/".
FORTRAN 77 requires that all parameters be passed by address.
Hence "value result" parameters are not supported.
The following syntax is illegal in FORTRAN 77.
.exam begin
      SUBROUTINE GRAPH(/NPTS/)
      ENTRY GRAPHS(/NPTS/)
.exam end
.np
Consider the following example.
.exam begin
      I=1
      CALL ASSOC( I, I )
      STOP
      END
      SUBROUTINE ASSOC( M, N )
      M = M + 1
      PRINT, M, N
      RETURN
      END
.exam end
.pc
When compiled and executed using WATFIV or WATFOR-11, the values
of
.id M
and
.id N,
at the PRINT statement, are 2 and 1 respectively.
When compiled and executed using &cmpname, the values of
.id M
and
.id N,
at the PRINT statement, are both 2.
.note
WATFIV/WATFOR-11 permit the association of a scalar variable
actual argument with a single-element array dummy argument.
.exam begin
      X = 5.0
      CALL SR( X )
      STOP
      END
      SUBROUTINE SR( A )
      DIMENSION A( 1 )
      A( 1 ) = A( 1 ) + 1.0
      RETURN
      END
.exam end
.pc
This is not legal in FORTRAN 77 and is not supported by &cmpname..
.note
WATFIV permits a CHARACTER or hollerith constant actual argument
to be associated with a CHARACTER array dummy argument.
.exam begin
      CALL SR1( 'MESSAGE' )
      CALL SR2( 'MESSAGE' )
      STOP
      END
      SUBROUTINE SR1( A )
      CHARACTER*7 A
      PRINT, A
      RETURN
      END
      SUBROUTINE SR2( A )
      CHARACTER*1 A( 7 )
      PRINT, A
      RETURN
      END
.exam end
.pc
In the above example, the CALL to subroutine SR1 is legal in
FORTRAN 77.
The CALL to subroutine SR2 is illegal since an attempt is made to
associate the CHARACTER constant 'MESSAGE' with an array.
&cmpname will issue a run-time error message when subroutine SR2 is
called.
.note
WATFIV/WATFOR-11 permit an actual argument array to be associated
with a dummy argument array of a different type.
.exam begin
      INTEGER I(2) /1, 2/
      CALL SR( I )
      STOP
      END
      SUBROUTINE SR( A )
      REAL A(2)
      PRINT, A
      RETURN
      END
.exam end
.exam begin
      REAL X(6) /1.0, 2.0, 3.0, 4.0, 5.0, 6.0/
      CALL SR( X )
      STOP
      END
      SUBROUTINE SR( C )
      COMPLEX C(3)
      PRINT, C
      RETURN
      END
.exam end
.pc
This is not legal in FORTRAN 77.
&cmpname will allow this only if the "NOARRAYCHECK" option is
specified.
.endnote
.np
There may be other incompatibilities between WATFIV/WATFOR-11 and
&cmpname..
We would be pleased to hear about any that we may not have documented.
