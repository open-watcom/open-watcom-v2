.func fegetenv
.synop begin
#include <fenv.h>
int fegetenv( fenv_t *__envp );
.ixfunc2 'Floating Point Environment' &func
.synop end
.*
.desc begin
The &func function attempts to store the current floating-point environment in the object
pointed to by envp.
.desc end
.*
.return begin
The &func function returns zero if the environment was successfully stored.
Otherwise, it returns a nonzero value.
.return end
.*
.see begin
.seelist &function. feholdexcept fesetenv feupdateenv
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
.class C99
.system
