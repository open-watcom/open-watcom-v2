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
function determines which of a specified subset of the floatingpoint
exception flags are currently set. The
.arg excepts
argument specifies the floating point status flags to be queried.
.desc end
.*
.return begin
The
.id &funcb.
function returns the value of the bitwise OR of the floating-point exception macros
corresponding to the currently set floating-point exceptions included in
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
