.func fesetenv
.synop begin
#include <fenv.h>
int fesetenv( const fenv_t *__envp );
.ixfunc2 'Floating Point Environment' &func
.synop end
.*
.desc begin
The &func function attempts to establishe the floating-point environment represented by the
object pointed to by envp. The argument envp shall point to an object set by a call to
.kw fegetenv
or
.kw feholdexcept
, or equal the
.kw FE_DFL_ENV
macro. Note that fesetenv merely installs the state of the floating-point status flags represented through
its argument, and does not raise these floating-point exceptions.
.desc end
.*
.return begin
The &func function returns zero if the environment was successfully established.
Otherwise, it returns a nonzero value.
.return end
.*
.see begin
.seelist &function. fegetenv feholdexcept feupdateenv
.see end
.*
.exmp begin
#include <fenv.h>
.exmp break
void main( void )
{
    fenv_t env;
    fegetenv( &env );
    fesetenv( FE_DFL_ENV );
    fesetenv( &env );
}
.exmp end
.class C99
.system
