.func fegetround
#include <fenv.h>
int fegetround( void );
.ixfunc2 'Floating Point Environment' &func
.synop end
.*
.desc begin
The &func function gets the current rounding direction.
.desc end
.*
.return begin
The &func function returns the value of the rounding direction macro
representing the current rounding direction or a negative value if there is no such
rounding direction macro or the current rounding direction is not determinable.
.np
Valid rounding modes are
.kw FE_TONEAREST
.kw FE_DOWNWARD
.kw FE_TOWARDZERO
.kw FE_UPWARD
.return end
.*
.see begin
.seelist &function. fesetround
.see end
.*
.exmp begin
#include <stdio.h>
#include <fenv.h>
.exmp break
void main( void )
{
    int mode;
    mode = fegetround();
    if ( mode == FE_TONEAREST )
	    printf( "Nearest\n" );
    else if ( mode == FE_DOWNWARD )
        printf( "Down\n" );
    else if ( mode == FE_TOWARDZERO )
        printf( "To Zero\n" );
    else if ( mode == FE_UPWARD )
        printf( "Up\n" );
}
.exmp end
.class C99
.system
