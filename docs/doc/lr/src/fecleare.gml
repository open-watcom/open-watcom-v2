.func feclearexcept
#include <fenv.h>
int feclearexcept( int __excepts );
.ixfunc2 'Floating Point Environment' &func
.synop end
.*
.desc begin
The &func function attempts to clear the supported floating-point exceptions
represented by its argument.
.desc end
.*
.return begin
The &func function returns zero if the excepts argument is zero or if all
the specified exceptions were successfully cleared. Otherwise, it returns a nonzero value.
.return end
.*
.see begin
.seelist &function. fegetexceptflag feraiseexcept fesetexceptflag fetestexcept
.see end
.*
.exmp begin
#include <fenv.h>
.exmp break
void main( void )
{
	feclearexcept( FE_OVERFLOW|FE_UNDERFLOW );
}
.exmp end
.class C99
.system
