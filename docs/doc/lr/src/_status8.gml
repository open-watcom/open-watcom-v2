.func _status87
.synop begin
#include <float.h>
unsigned int _status87( void );
.ixfunc2 '&Errs' &func
.synop end
.desc begin
The
.id &func.
function returns the floating-point status word which is used
to record the status of 8087/80287/80387/80486 floating-point operations.
.desc end
.return begin
The
.id &func.
function returns the floating-point status word which is
used to record the status of 8087/80287/80387/80486 floating-point
operations.
The description of this status is found in the
.hdrfile float.h
header file.
.return end
.see begin
.seelist _clear87 _control87 _controlfp _finite _fpreset _status87
.see end
.exmp begin
#include <stdio.h>
#include <float.h>

#define TEST_FPU(x,y) printf( "\t%s " y "\n", \
                ((fp_status & x) ? "  " : "No") )
.exmp break
void main()
  {
    unsigned int fp_status;

    fp_status = _status87();
.exmp break
    printf( "80x87 status\n" );
    TEST_FPU( SW_INVALID, "invalid operation" );
    TEST_FPU( SW_DENORMAL, "denormalized operand" );
    TEST_FPU( SW_ZERODIVIDE, "divide by zero" );
    TEST_FPU( SW_OVERFLOW, "overflow" );
    TEST_FPU( SW_UNDERFLOW, "underflow" );
    TEST_FPU( SW_INEXACT, "inexact result" );
  }
.exmp end
.class Intel
.system
