.func _clear87
.synop begin
#include <float.h>
unsigned int _clear87( void );
.ixfunc2 '&Errs' &func
.synop end
.desc begin
The &func function clears the floating-point status word which is used
to record the status of 8087/80287/80387/80486 floating-point operations.
.desc end
.return begin
The &func function returns the old floating-point status.
The description of this status is found in the
.hdrfile float.h
header file.
.return end
.see begin
.seelist &function. _clear87 _control87 _controlfp _finite _fpreset _status87
.see end
.exmp begin
#include <stdio.h>
#include <float.h>
.exmp break
void main()
  {
    unsigned int fp_status;
.exmp break
    fp_status = _clear87();
.exmp break
    printf( "80x87 status =" );
    if( fp_status & SW_INVALID )
        printf( " invalid" );
    if( fp_status & SW_DENORMAL )
        printf( " denormal" );
    if( fp_status & SW_ZERODIVIDE )
        printf( " zero_divide" );
    if( fp_status & SW_OVERFLOW )
        printf( " overflow" );
    if( fp_status & SW_UNDERFLOW )
        printf( " underflow" );
    if( fp_status & SW_INEXACT )
        printf( " inexact_result" );
    printf( "\n" );
  }
.exmp end
.class Intel
.system
