typedef struct ld {
        unsigned short x[4];
        unsigned short exponent;
} LD;
#pragma aux     __FLDA "*";
#pragma aux     __FLDS "*";
#pragma aux     __FLDC "*";
#pragma aux     __FLDM "*";
#pragma aux     __FLDD "*";
#pragma aux     __FLDN "*";
#pragma aux     __FDLD "*" parm caller [ax bx cx dx] [si];
#pragma aux     __LDFD "*";
#pragma aux     __I4LD "*";
#pragma aux     __U4LD "*";
#pragma aux     __LDI4 "*";
#pragma aux     __LDU4 "*";
#pragma aux     __LDFS "*";
#pragma aux     __FSLD "*";
#pragma aux     __sqrt "*";
#pragma aux     __log  "*";
#pragma aux     __sin  "*";
#pragma aux     __cos  "*";
#pragma aux     __tan  "*";
#pragma aux     __atan "*";
#pragma aux     __f2xm1 "*";
#pragma aux     __fyl2x "*";
#pragma aux     __fprem "*";
#pragma aux     __EvalPoly "*";
#pragma aux     __OddPoly  "*";
#pragma aux     __Poly  "*";

void    __FLDA( LD *, LD *, LD * );     // add
void    __FLDS( LD *, LD *, LD * );     // subtract
void    __FLDM( LD *, LD *, LD * );     // multiply
void    __FLDD( LD *, LD *, LD * );     // divide
int     __FLDC( LD *, LD * );           // compare
void    __FLDN( LD * );                 // negate
void    __FDLD( double, LD * );         // convert double to long double
double  __LDFD( LD * );                 // convert long double to double
void    __I4LD( long, LD * );           // convert long to long double
void    __U4LD( long, LD * );           // convert unsigned long to long double
long    __LDI4( LD * );                 // convert long double to long
unsigned long __LDU4( LD * );           // convert long double to unsigned long
float   __LDFS( LD * );                 // convert long double to float
void    __FSLD( float, LD * );          // convert float to long double
int     __fprem( LD *, LD * );          // fprem
int     __fyl2x( LD *, LD * );          // fyl2x
void    __sqrt( LD * );                 // fsqrt
void    __log( LD * );                  // log
void    __sin( LD * );                  // sin
void    __cos( LD * );                  // cos
void    __tan( LD * );                  // tan
void    __atan( LD * );                 // atan
void    __f2xm1( LD * );                // 2**x -1
