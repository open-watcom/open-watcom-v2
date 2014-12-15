.func fesetexceptflag
.synop begin
#include <fenv.h>
int fesetexceptflag( const fexcept_t *__flagp, int __excepts );
.ixfunc2 'Floating Point Environment' &funcb
.synop end
.*
.desc begin
The
.id &funcb.
function attempts to set the floating-point status flags indicated by the
argument excepts to the states stored in the object pointed to by flagp. The value of
*flagp shall have been set by a previous call to
.kw fegetexceptflag
whose second argument represented at least those floating-point exceptions represented by the argument
excepts. This function does not raise floating-point exceptions, but only sets the state
of the flags.
.desc end
.*
.return begin
The
.id &funcb.
function returns zero if the excepts argument is zero or if
all the specified flags were successfully set to the appropriate state. Otherwise, it returns
a nonzero value.
.return end
.*
.see begin
.seelist feclearexcept fegetexceptflag feraisexcept fetestexcept
.see end
.*
.exmp begin
#include <fenv.h>
.exmp break
void main( void )
{
    fexcept_t flags;
    fgetexceptflag( &flags, FE_DENORMAL|FE_INVALID );
    fsetexceptflag( &flags, FE_INVALID );
}
.exmp end
.class ISO C99
.system
