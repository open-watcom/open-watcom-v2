.func feholdexcept
.synop begin
#include <fenv.h>
int feholdexcept( fenv_t *envp );
.ixfunc2 'Floating Point Environment' &funcb
.synop end
.*
.desc begin
The
.id &funcb.
function saves the current floating-point environment in the object
pointed to by
.arg envp
argument, clears the floating-point status flags, and then installs a non-stop
(continue on floating-point exceptions) mode, if available, for all floating-point
exceptions.
.desc end
.*
.return begin
The
.id &funcb.
function returns zero if and only if non-stop floating-point
exception handling was successfully installed.
.return end
.*
.see begin
.seelist fegetenv fesetenv feupdateenv
.see end
.*
.exmp begin
#include <fenv.h>
.exmp break
void main( void )
{
    fenv_t env;
    feholdexcept( &env );
}
.exmp end
.class ISO C99
.system
