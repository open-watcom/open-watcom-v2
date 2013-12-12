.func __feenableexcept
.synop begin
#include <fenv.h>
void __feenableexcept( int __excepts );
.ixfunc2 'Floating Point Environment' &func
.synop end
.*
.desc begin
The &func function enables the specified floating point exceptions.
.desc end
.*
.return begin
No value is returned.
.return end
.*
.see begin
.seelist &function. __fedisableexcept
.see end
.*
.exmp begin
#include <fenv.h>
.exmp break
void main( void )
{
    __feenableexcept( FE_DIVBYZERO );
}
.exmp end
.class WATCOM
.system
