.func feenableexcept __feenableexcept
.synop begin
#include <fenv.h>
void feenableexcept( int __excepts );
void __feenableexcept( int __excepts );
.ixfunc2 'Floating Point Environment' &funcb
.synop end
.*
.desc begin
The
.id &funcb.
function enables the specified floating point exceptions.
.desc end
.*
.return begin
No value is returned.
.return end
.*
.see begin
.seelist fedisableexcept __fedisableexcept
.see end
.*
.exmp begin
#include <fenv.h>
.exmp break
void main( void )
{
    feenableexcept( FE_DIVBYZERO );
}
.exmp end
.class WATCOM
.system
