.func fetestexcept
.synop begin
#include <fenv.h>
int fetestexcept( int excepts );
.ixfunc2 'Floating Point Environment' &funcb
.synop end
.*
.desc begin
The
.id &funcb.
function tests which of the specified floating-point
exceptions flags are currently set. The
.arg excepts
argument specifies the floating-point exceptions to be queried.
.np
For valid exception values see
.seekw &function. fegetexceptflag
.ct .li .
.desc end
.*
.return begin
The
.id &funcb.
function returns the value of the bitwise OR of the floating-point exception macros
corresponding to the currently set floating-point exceptions included in the
.arg excepts
argument.
.return end
.*
.see begin
.seelist feclearexcept fegetexceptflag feraiseexcept fesetexceptflag
.see end
.*
.exmp begin
#include <stdio.h>
#include <fenv.h>
.exmp break
void main( void )
{
    int excepts;
    feclearexcept( FE_DIVBYZERO );
.exmp break
    ...code that may cause a divide by zero exception
.exmp break
    excepts = fetestexcept( FE_DIVBYZERO );
    if ( excepts & FE_DIVBYZERO)
        printf( "Divide by zero occurred\n" );
}
.exmp end
.class ISO C99
.system
