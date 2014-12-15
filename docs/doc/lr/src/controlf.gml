.func _controlfp
.synop begin
#include <float.h>
unsigned int _controlfp( unsigned int newcw,
                         unsigned int mask );
.ixfunc2 '&Errs' &funcb
.synop end
.desc begin
The
.id &funcb.
function updates the control word of the
8087/80287/80387/80486.
If
.arg mask
is zero, then the control word is not updated.
If
.arg mask
is non-zero, then the control word is updated with bits from
.arg newcw
corresponding to every bit that is on in
.arg mask
.ct .li .
.desc end
.return begin
The
.id &funcb.
function returns the new control word.
The description of bits defined for the control word is found in the
.hdrfile float.h
header file.
.return end
.see begin
.seelist _clear87 _control87 _controlfp _finite _fpreset _status87
.see end
.exmp begin
#include <stdio.h>
#include <float.h>

char *status[2] = { "disabled", "enabled" };

void main()
  {
    unsigned int fp_cw = 0;
    unsigned int fp_mask = 0;
    unsigned int bits;
.exmp break
    fp_cw = _controlfp( fp_cw,
                        fp_mask );
.exmp break
    printf( "Interrupt Exception Masks\n" );
    bits = fp_cw & MCW_EM;
    printf( "  Invalid Operation exception %s\n",
            status[ (bits & EM_INVALID) == 0 ] );
    printf( "  Denormalized exception %s\n",
            status[ (bits & EM_DENORMAL) == 0 ] );
    printf( "  Divide-By-Zero exception %s\n",
            status[ (bits & EM_ZERODIVIDE) == 0 ] );
    printf( "  Overflow exception %s\n",
            status[ (bits & EM_OVERFLOW) == 0 ] );
    printf( "  Underflow exception %s\n",
            status[ (bits & EM_UNDERFLOW) == 0 ] );
    printf( "  Precision exception %s\n",
            status[ (bits & EM_PRECISION) == 0 ] );
.exmp break
    printf( "Infinity Control = " );
    bits = fp_cw & MCW_IC;
    if( bits == IC_AFFINE )     printf( "affine\n" );
    if( bits == IC_PROJECTIVE ) printf( "projective\n" );
.exmp break
    printf( "Rounding Control = " );
    bits = fp_cw & MCW_RC;
    if( bits == RC_NEAR )       printf( "near\n" );
    if( bits == RC_DOWN )       printf( "down\n" );
    if( bits == RC_UP )         printf( "up\n" );
    if( bits == RC_CHOP )       printf( "chop\n" );
.exmp break
    printf( "Precision Control = " );
    bits = fp_cw & MCW_PC;
    if( bits == PC_24 )         printf( "24 bits\n" );
    if( bits == PC_53 )         printf( "53 bits\n" );
    if( bits == PC_64 )         printf( "64 bits\n" );
  }
.exmp end
.class Intel
.system
