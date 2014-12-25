.func fedisableexcept __fedisableexcept
.synop begin
#include <fenv.h>
void fedisableexcept( int excepts );
void __fedisableexcept( int excepts );
.ixfunc2 'Floating Point Environment' &funcb
.synop end
.*
.desc begin
The
.id &funcb.
function disables the floating point exceptions specified by
.arg excepts
argument.
.
.desc end
.*
.return begin
No value is returned.
.return end
.*
.see begin
.seelist feenableexcept
.see end
.*
.exmp begin
#include <fenv.h>
.exmp break
void main( void )
{
    fedisableexcept( FE_DIVBYZERO );
}
.exmp end
.class WATCOM
.system
