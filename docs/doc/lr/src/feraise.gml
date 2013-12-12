.func feraiseexcept
.synop begin
#include <fenv.h>
int feraiseexcept( int __excepts );
.ixfunc2 'Floating Point Environment' &func
.synop end
.*
.desc begin
The
.id &func.
function attempts to raise the supported floating-point exceptions
represented by its argument.
.desc end
.*
.return begin
The
.id &func.
function returns zero if the excepts argument is zero or if all
the specified exceptions were successfully raised. Otherwise, it returns a nonzero value.
.return end
.*
.see begin
.seelist &function. feclearexcept fegetexceptflag fetestexcept
.see end
.*
.exmp begin
#include <fenv.h>
.exmp break
void main( void )
{
	feraiseexcept( FE_DIVBYZERO );
}
.exmp end
.class C99
.system
