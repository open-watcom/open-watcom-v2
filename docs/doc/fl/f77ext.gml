.chap &product Extensions to Standard FORTRAN 77
.*
.np
.ix 'extensions' 'language'
.ix 'extensions' 'summary'
.ix 'FORTRAN 77' 'language extensions'
This appendix summarizes the extensions supported by &product..
.autonote
.note
The
.kw INCLUDE
statement for embedding source from another file is supported.
.millust begin
      INCLUDE 'SRC'
.millust end
.note
Symbolic names are unique up to 32 characters.
Also, '$', '_', and lowercase letters are allowed in symbolic names.
.note
Lowercase and uppercase letters are treated in the same way
except in:
.autopoint
.point
character and hollerith constants
.point
apostrophe and H edit descriptors
.endpoint
.note
End-of-line comments are permitted.
.millust begin
      PRINT *, 'Hello world'  ! print 'Hello World'
.millust end
.note
The
.kw IMPLICIT NONE
statement is supported.
.note
An asterisk enclosed in parentheses is allowed with the type
.kw CHARACTER
when specified in an
.kw IMPLICIT
statement.
.millust begin
      IMPLICIT CHARACTER*(*) (Z)
.millust end
.note
Length specifiers are allowed with types specified in
.kw IMPLICIT
statements.
.millust begin
      IMPLICIT INTEGER*2 (I-N)
.millust end
.note
Length specifiers are allowed with type specification statements.
.millust begin
      LOGICAL*1, LOGICAL*4
      INTEGER*1, INTEGER*2, INTEGER*4
      REAL*4, REAL*8
      COMPLEX*8, COMPLEX*16
.millust end
.np
Length specifiers are also allowed with the type specified in
.kw FUNCTION
statements.
.millust begin
      COMPLEX*16 FUNCTION ZADD( X, Y )
.millust end
.note
Length specifiers are allowed with symbol names.
.millust begin
      INTEGER I*2, A*2(10), B(20)*2

      COMPLEX FUNCTION ZADD*16( X, Y )
.millust end
.note
The
.kw DOUBLE COMPLEX
statement is supported (equivalent to
.kw COMPLEX*16
.ct ).
.note
Double precision complex constants are allowed.
.millust begin
      Z = (1D0, 2D0)
.millust end
.note
Mixing operands of type
.kw DOUBLE PRECISION
and
.kw COMPLEX
to yield a
.kw COMPLEX*16
result is allowed.
.millust begin
      DOUBLE PRECISION X
      COMPLEX Y, Z*16
      Z = X + Y
.millust end
.note
User-defined structures are supported.
.millust begin
      STRUCTURE/END STRUCTURE
      UNION/END UNION
      MAP/END MAP
      RECORD
.millust end
.note
Both character and non-character data are allowed in the same
common block.
.millust begin
      INTEGER X
      CHARACTER C
      COMMON /BLK/ X, C
.millust end
.note
Data initialization of variables in common without a block data
subprogram is allowed.
.note
Equivalencing character to non-character data is permitted.
.millust begin
      INTEGER X
      CHARACTER C
      EQUIVALENCE (X, C)
.millust end
.note
Single subscripts for multi-dimensional arrays is permitted in
.kw EQUIVALENCE
statements.
.note
Data initialization in a type specification statement is allowed.
.millust begin
      DOUBLE PRECISION X/4.3D1/
.millust end
.note
Data initialization with hexadecimal constants is allowed.
.millust begin
      INTEGER I/Z00000007/
.millust end
.note
Initializing character items with numeric data is permitted.
.note
Hexadecimal and octal constants of the form
.mono 'abc'x
and
.mono '567'o
are supported.
.note
A character constant of the form
.mono 'abcdef'c
places a NUL character (CHAR(0)) at the end of the character string.
.note
Hollerith constants can be used interchangeably with character constants.
.millust begin
      CHARACTER*10 A, B
      A = '1234567890'
      B = 10H123456790
.millust end
.note
Several additional intrinsic functions are supported:
.millust begin
ALGAMA        ALLOCATED     BTEST         CDABS
CDCOS         CDSIN         CDEXP         CDSQRT
CDLOG         COTAN         DCMPLX        DCONJG
DCOTAN        DERF          DERFC         DFLOAT
DGAMMA        DIMAG         DLGAMA        DREAL
ERF           ERFC          GAMMA         HFIX
IAND          IBCHNG        IBCLR         IBSET
IEOR          IOR           ISHA          ISHC
ISHFT         ISHL          ISIZEOF       LENTRIM
LGAMMA        LOC           NOT           VOLATILE
.millust end
.note
The
.kw LOC
intrinsic function returns the address of an expression.
.note
The
.kw ISIZEOF
intrinsic function returns the size of a structure name, the size of
an array with a constant array declarator, or the size of a variable.
.note
The
.kw CHAR
intrinsic function is allowed in constant expressions.
.note
The
.kw ALLOCATE
and
.kw DEALLOCATE
statements may be used to dynamically allocate and deallocate arrays.
.note
The
.kw ALLOCATED
intrinsic function may be used to determine if an allocatable array is
allocated.
.note
The following additional I/O specifiers for the
.kw OPEN
statement are supported.
.millust begin
ACTION=
CARRIAGECONTROL=
RECORDTYPE=
RECL= is also allowed for files opened for
      sequential access
ACCESS='APPEND'
BLOCKSIZE=
SHARE=
.millust end
.note
The following additional I/O specifiers for the
.kw INQUIRE
statement are supported.
.millust begin
ACTION=
CARRIAGECONTROL=
RECORDTYPE=
BLOCKSIZE=
SHARE=
.millust end
.note
In the
.kw INQUIRE
statement, character data may also be returned in variables or array
elements with a substring operation.
.millust begin
      CHARACTER FN*20
      INQUIRE( UNIT=1, FILE=FN(10:20) )
.millust end
.note
List-directed I/O is allowed with internal files.
.note
No asterisk is required for list-directed I/O.
.millust begin
      PRINT, X, Y
.millust end
.note
The
.kw NAMELIST
statement is supported.
.note
Non-character arrays are allowed as format specifiers.
.note
The following format edit descriptors are allowed:
.begnote
.note Z
for displaying data in hexadecimal format
.note Ew.dDe
same as Ew.dEe except D is used as exponentiation character
.note $ or \
leave cursor at end of line
.endnote
.note
A repeat count is not required for the X edit descriptor (a repeat
count of one is assumed).
.note
Commas are optional between format edit descriptors.
.millust begin
100   FORMAT( 1X I5 )
.millust end
.note
It is possible to substring the return values of functions and
statement functions.
.millust begin
      CHARACTER*7 F, G
      F() = '1234567'
      PRINT *, F()(1:3), G()(4:7)
.millust end
.note
Functions may be invoked via the
.kw CALL
statement.
This allows the return value of functions to be ignored.
.note
A
.kw RETURN
statement is allowed in the main program.
.note
Integer constants with more than 5 digits are allowed in the
.kw STOP
and
.kw PAUSE
statements.
.millust begin
      PAUSE 123456

      STOP 123456
.millust end
.note
Multiple assignment is allowed.
.millust begin
      X = Y = Z = 0.0
.millust end
.note
The
.kw .XOR.
operator is supported (equivalent to
.mono .NEQV.
.ct ).
.note
The
.kw .AND.
.ct ,
.kw .OR.
.ct ,
.kw .NEQV.
.ct ,
.kw .EQV.
and
.kw .XOR.
operators may take integer arguments.
They can be used to perform bit operations on integers.
.note
Several additional program structure control statements are supported:
.millust begin
      LOOP-ENDLOOP
      UNTIL (can be used with WHILE and LOOP)
      WHILE-ENDWILE
      GUESS-ADMIT-ENDGUESS
      ATENDDO-ENDATEND
      ATEND
      SELECT-ENDSELECT
      DOWHILE-ENDDO
      DO-ENDDO (no statement number)
      REMOTEBLOCK-ENDBLOCK
      EXECUTE
      QUIT
      EXIT
      CYCLE
.millust end
.note
Block labels can be used to identify blocks of code.
.millust begin
      LOOP : OUTER_LOOP
          <statements>
          LOOP : INNER_LOOP
              <statements>
              IF( X .GT.100 ) QUIT : OUTER_LOOP
              <statements>
          ENDLOOP
          <statements>
      ENDLOOP
.millust end
.note
An integer expression in an
.kw IF
.ct ,
.kw ELSE IF
.ct ,
.kw DO WHILE
.ct ,
.kw WHILE
or
.kw UNTIL
statement is allowed.
The result of the integer expression is compared for inequality to the
integer value 0.
.endnote
