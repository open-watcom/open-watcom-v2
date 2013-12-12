.func _fpreset
.synop begin
#include <float.h>
void _fpreset( void );
.ixfunc2 '&Errs' &func
.synop end
.*
.desc begin
The &func function resets the floating-point unit to the default state that
the math library requires for correct function. After a floating-point
exception, it may be necessary to call the &func function before any further
floating-point operations are attempted.
.np
In multi-threaded environments, &func only affects the current thread.
.desc end
.*
.return begin
No value is returned.
.return end
.*
.see begin
.seelist &function. _clear87 _control87 _controlfp _finite _fpreset _status87
.see end
.*
.exmp begin
#include <stdio.h>
#include <float.h>
.exmp break
char *status[2] = { "No", "  " };
.exmp break
void main( void )
{
    unsigned int fp_status;
.exmp break
    fp_status = _status87();
.exmp break
    printf( "80x87 status\n" );
    printf( "%s invalid operation\n",
            status[ (fp_status & SW_INVALID) == 0 ] );
    printf( "%s denormalized operand\n",
            status[ (fp_status & SW_DENORMAL) == 0 ] );
    printf( "%s divide by zero\n",
            status[ (fp_status & SW_ZERODIVIDE) == 0 ] );
    printf( "%s overflow\n",
            status[ (fp_status & SW_OVERFLOW) == 0 ] );
    printf( "%s underflow\n",
            status[ (fp_status & SW_UNDERFLOW) == 0 ] );
    printf( "%s inexact result\n",
            status[ (fp_status & SW_INEXACT) == 0 ] );
    _fpreset();
}
.exmp end
.class Intel
.system
