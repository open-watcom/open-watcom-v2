.section EXTERNAL Statement
.*
.mbox begin
      EXTERNAL p [,p] ...
.mbox end
.synote
.mnote p
is the name of an external procedure, dummy procedure or block data
subprogram.
.endnote
.np
The
.kw EXTERNAL
statement identifies a symbolic name to be a dummy procedure or an
external procedure and allows these names to be passed as an actual
argument.
In the following example,
.id SAM,
.id ERRRTN
and
.id POLY
are declared to be external procedures.
.exam begin
      EXTERNAL SAM, ERRRTN, POLY
.exam end
.np
In the following example,
.id F
is declared to be an external procedure and is passed as such to
subroutine
.id SAM.
If the
.kw EXTERNAL
statement were eliminated then the variable
.id F
would be passed on
to subroutine
.id SAM
since there is no way of knowing that
.id F
is an external function.
.exam begin
      EXTERNAL F
          .
          .
          .
      CALL SAM( F )
.exam end
.np
The appearance of an intrinsic function in an
.kw EXTERNAL
statement declares that name to be an external procedure and the
intrinsic function by that name is no longer available in that program
unit.
This allows the programmer to define a function by the same name as an
intrinsic function.
In the following example, the programmer's
.id SIN
function will be called instead of the intrinsic
.id SIN
function.
.exam begin
      EXTERNAL SIN
          .
          .
          .
      CALL SIN( .1 )
.exam end
.np
A statement function name must not appear in an
.kw EXTERNAL
statement.
A name must only appear in an
.kw EXTERNAL
statement once.
