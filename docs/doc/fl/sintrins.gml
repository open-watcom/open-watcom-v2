.section INTRINSIC Statement
.*
.mbox begin
      INTRINSIC f [,f] ...
.mbox end
.synote
.mnote f
is the name of an intrinsic function name.
.endnote
.np
An
.kw INTRINSIC
statement is used to identify a symbolic name as the name of an
intrinsic function.
It also allows a specific intrinsic function to
be passed as an actual argument.
The names of intrinsic functions for type conversion (INT, IFIX, HFIX,
IDINT, FLOAT, DFLOAT, SNGL, REAL, DREAL, DBLE, CMPLX, DCMPLX, ICHAR,
CHAR), lexical relationship (LGE, LGT, LLE, LLT), for choosing the
largest or smallest value (MAX, MAX0, AMAX1, DMAX1, AMAX0, MAX1, MIN,
MIN0, AMIN1, DMIN1, AMIN0, MIN1), as well as ALLOCATED, ISIZEOF and
LOC, must not be used as actual arguments.
.np
A generic intrinsic function does not lose its generic property if
it appears in an
.kw INTRINSIC
statement.
.np
A name must only appear in an
.kw INTRINSIC
statement once.
A symbolic name must not appear in both an
.kw INTRINSIC
and an
.kw EXTERNAL
statement in a program unit.
.exam begin
      INTRINSIC SIN
          .
          .
          .
      CALL SAM( SIN )
.exam end
.pc
In the previous example, the intrinsic function
.id SIN
was passed to the subroutine
.id SAM.
If the
.kw INTRINSIC
statement were eliminated then the variable
.id SIN
would be passed to the subroutine
.id SAM.
