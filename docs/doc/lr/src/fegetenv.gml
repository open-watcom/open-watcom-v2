.func fegetenv
.synop begin
#include <fenv.h>
int fegetenv( fenv_t *__envp );
.ixfunc2 'Floating Point Environment' &funcb
.synop end
.*
.desc begin
The
.id &funcb.
function attempts to store the current floating-point environment in the object
pointed to by envp.
.desc end
.*
.return begin
The
.id &funcb.
function returns zero if the environment was successfully stored.
Otherwise, it returns a nonzero value.
.return end
.*
.see begin
.seelist feholdexcept fesetenv feupdateenv
.see end
.*
.exmp begin
#include <stdio.h>
#include <fenv.h>
.exmp break
void main( void )
{
    fenv_t env;
    fegetenv( &env );
}
.exmp end
.class ISO C99
.system
