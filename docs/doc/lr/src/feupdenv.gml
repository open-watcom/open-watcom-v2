.func feupdateenv
#include <fenv.h>
int feupdateenv( const fenv_t *__envp );
.ixfunc2 'Floating Point Environment' &func
.funcend
.*
.desc begin
The &func function attempts to save the currently raised floating-point exceptions in its
automatic storage, installs the floating-point environment represented by the object
pointed to by envp, and then raises the saved floating-point exceptions. The argument
envp shall point to an object set by a call to feholdexcept or fegetenv, or equal a
floating-point environment macro.
.desc end
.*
.return begin
The &func function returns zero if all the actions were successfully carried out.
Otherwise, it returns a nonzero value.
.return end
.*
.see begin
.seelist &function. fegetenv feholdexcept fesetenv
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
    feupdateenv( &env );
}
.exmp end
.class C99
.system
