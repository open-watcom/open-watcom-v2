.func fegetexceptflag
.synop begin
#include <fenv.h>
int fegetexceptflag( fexcept_t *flagp, int excepts );
.ixfunc2 'Floating Point Environment' &funcb
.synop end
.*
.desc begin
The
.id &funcb.
function attempts to store a representation of the states of the
floating-point status flags indicated by the
.arg excepts
argument in the
object pointed to by the
.arg flagp
argument.
.np
Valid exception values are
.begnote $setptnt 13
.note FE_INVALID
.ix FE_INVALID
At least one of the arguments is a value for which the function is not defined.
.note FE_DENORMAL
.ix FE_DENORMAL
The result is not normalized.
.note FE_DIVBYZERO
.ix FE_DIVBYZERO
Division by zero.
.note FE_OVERFLOW
.ix FE_OVERFLOW
The result is too large in magnitude to be represented as the return type.
.note FE_UNDERFLOW
.ix FE_UNDERFLOW
The result is too small in magnitude to be represented as the return type.
.note FE_INEXACT
.ix FE_INEXACT
The result is not exact.
.note FE_ALL_EXCEPT
.ix FE_ALL_EXCEPT
Is the logical OR of all exceptions.
.endnote
.desc end
.*
.return begin
The
.id &funcb.
function returns zero if the representation was successfully
stored. Otherwise, it returns a nonzero value.
.return end
.*
.see begin
.seelist feclearexcept feraiseexcept fesetexceptflag fetestexcept
.see end
.*
.exmp begin
#include <fenv.h>
.exmp break
void main( void )
{
    fexcept_t flags;
    fegetexceptflag( &flags, FE_DIVBYZERO );
}
.exmp end
.class ISO C99
.system
