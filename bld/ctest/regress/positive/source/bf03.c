#include "fail.h"

/* Make sure that 'large' types can be used as a base for bit-fields */
typedef unsigned long base;
typedef signed long sbase;
typedef unsigned long long lbase;
typedef signed long long slbase;

struct X1 {
    lbase  f0 : 1;
    lbase  f1 : 9;
    lbase  f2 : 20;
    lbase  f3 : 9;   // crosses 32-bit boundary
    lbase  f4 : 1;
    lbase  f5 : 8;
    lbase  f6 : 1;
    lbase  f7 : 1;
    int m;
};

struct X1 x1 = { 0, 256, 755, 511, 0, 100, 0, 1, -1 };
struct X1 x1a;

void check_X1( struct X1 x1 )
{
    if( x1.f0 != 0   ) fail(__LINE__);
    if( x1.f1 != 256 ) fail(__LINE__);
    if( x1.f2 != 755 ) fail(__LINE__);
    if( x1.f3 != 511 ) fail(__LINE__);
    if( x1.f4 != 0   ) fail(__LINE__);
    if( x1.f5 != 100 ) fail(__LINE__);
    if( x1.f6 != 0   ) fail(__LINE__);
    if( x1.f7 != 1   ) fail(__LINE__);
    if( x1.m != -1   ) fail(__LINE__);
};

void set_X1a( void )
{
    x1a.f0 = 0;
    x1a.f1 = 256;
    x1a.f2 = 755;
    x1a.f3 = 511;
    x1a.f4 = 0;
    x1a.f5 = 100;
    x1a.f6 = 0;
    x1a.f7 = 1;
    x1a.m = -1;
}

void check_X1a( void )
{
    set_X1a();
    if( x1a.f0 != 0   ) fail(__LINE__);
    if( x1a.f1 != 256 ) fail(__LINE__);
    if( x1a.f2 != 755 ) fail(__LINE__);
    if( x1a.f3 != 511 ) fail(__LINE__);
    if( x1a.f4 != 0   ) fail(__LINE__);
    if( x1a.f5 != 100 ) fail(__LINE__);
    if( x1a.f6 != 0   ) fail(__LINE__);
    if( x1a.f7 != 1   ) fail(__LINE__);
    if( x1a.m != -1   ) fail(__LINE__);
};

struct X2 {
    sbase  f0 : 15;
    sbase  f1 : 20; //30;
    sbase  f2 : 2;
    sbase  f3 : 2;
    sbase  f4 : 2;
    sbase  f5 : 2;
    sbase  f6 : 2;
    sbase  f7 : 2;
    sbase  f8 : 2;
    int m;
};

struct X2 x2 = { 0, 1, 0, 1, 0, 1, 0, 1, 1, -1 };

void check_X2( struct X2 x2 )
{
    if( x2.f0 != 0 ) fail(__LINE__);
    if( x2.f1 != 1 ) fail(__LINE__);
    if( x2.f2 != 0 ) fail(__LINE__);
    if( x2.f3 != 1 ) fail(__LINE__);
    if( x2.f4 != 0 ) fail(__LINE__);
    if( x2.f5 != 1 ) fail(__LINE__);
    if( x2.f6 != 0 ) fail(__LINE__);
    if( x2.f7 != 1 ) fail(__LINE__);
    if( x2.f8 != 1 ) fail(__LINE__);
    if( x2.m != -1 ) fail(__LINE__);
};

struct X3 {
    base f0 : 1;
    base f1 : 1;
    base f2 : 1;
    base f3 : 1;
    unsigned f4 : 1;
    unsigned f5 : 1;
    unsigned f6 : 1;
    unsigned f7 : 1;
    unsigned f8 : 1;
    int m;
};

struct X3 x3 = { 0, 1, 0, 1, 0, 1, 0, 1, 1, -1 };

void check_X3( struct X3 x3 )
{
    if( x3.f0 != 0 ) fail(__LINE__);
    if( x3.f1 != 1 ) fail(__LINE__);
    if( x3.f2 != 0 ) fail(__LINE__);
    if( x3.f3 != 1 ) fail(__LINE__);
    if( x3.f4 != 0 ) fail(__LINE__);
    if( x3.f5 != 1 ) fail(__LINE__);
    if( x3.f6 != 0 ) fail(__LINE__);
    if( x3.f7 != 1 ) fail(__LINE__);
    if( x3.f8 != 1 ) fail(__LINE__);
    if( x3.m != -1 ) fail(__LINE__);
};

struct X4 {
    base f0 : 1;
    base f1 : 1;
    base f2 : 1;
    base f3 : 1;
    int m;
    base f4 : 1;
    base f5 : 1;
    base f6 : 1;
    base f7 : 1;
    int n;
    base f8 : 1;
};

struct X4 x4 = { 0, 1, 0, 1, -1, 0, 1, 0, 1, -1, 1 };

void check_X4( struct X4 x4 )
{
    if( x4.f0 != 0 ) fail(__LINE__);
    if( x4.f1 != 1 ) fail(__LINE__);
    if( x4.f2 != 0 ) fail(__LINE__);
    if( x4.f3 != 1 ) fail(__LINE__);
    if( x4.m != -1 ) fail(__LINE__);
    if( x4.f4 != 0 ) fail(__LINE__);
    if( x4.f5 != 1 ) fail(__LINE__);
    if( x4.f6 != 0 ) fail(__LINE__);
    if( x4.f7 != 1 ) fail(__LINE__);
    if( x4.n != -1 ) fail(__LINE__);
    if( x4.f8 != 1 ) fail(__LINE__);
};

struct X5 {
    base f0 : 1;
    base f1 : 1;
    base f2 : 1;
    base f3 : 1;
    int m;
    base f4 : 1;
    base f5 : 1;
    base f6 : 1;
    base f7 : 1;
    int n;
    base f8 : 1;
    struct X1 x1;
    struct X2 x2;
    struct X3 x3;
    struct X4 x4;
};

struct X5 x5 = { 0, 1, 0, 1, -1, 0, 1, 0, 1, -1, 1,
    { 0, 256, 755, 511, 0, 100, 0, 1, -1 },
    { 0, 1, 0, 1, 0, 1, 0, 1, 1, -1 },
    { 0, 1, 0, 1, 0, 1, 0, 1, 1, -1 },
    { 0, 1, 0, 1, -1, 0, 1, 0, 1, -1, 1 },
};

void check_X5( struct X5 x5 )
{
    if( x5.f0 != 0 ) fail(__LINE__);
    if( x5.f1 != 1 ) fail(__LINE__);
    if( x5.f2 != 0 ) fail(__LINE__);
    if( x5.f3 != 1 ) fail(__LINE__);
    if( x5.m != -1 ) fail(__LINE__);
    if( x5.f4 != 0 ) fail(__LINE__);
    if( x5.f5 != 1 ) fail(__LINE__);
    if( x5.f6 != 0 ) fail(__LINE__);
    if( x5.f7 != 1 ) fail(__LINE__);
    if( x5.n != -1 ) fail(__LINE__);
    if( x5.f8 != 1 ) fail(__LINE__);
    check_X1( x5.x1 );
    check_X2( x5.x2 );
    check_X3( x5.x3 );
    check_X4( x5.x4 );
};

struct X6 {
    sbase   f0 : 20;
    sbase   f1 : 20;
//    sbase   f2 : 40;  TODO - some other day
//    sbase   f3 : 40;
};

struct X6 x6 = { 0xC0000, 0x90000 };

long op_x6( struct X6 x6 )
{
    return( x6.f0 & x6.f1 );    // This was a problem in 16-bit
}

void check_X6( struct X6 x6 )
{
    if( op_x6( x6 ) != 0xFFF80000 ) fail(__LINE__);
}

struct X7 {
    slbase f0 : 1;
    slbase f1 : 9;
    slbase f2 : 20;
    slbase f3 : 9;   // crosses 32-bit boundary
    slbase f4 : 1;
    slbase f5 : 8;
    slbase f6 : 1;
    slbase f7 : 1;
    int m;
};

/* As a side effect, check that unsigned integers are properly
 * converted into signed values.
 */
struct X7 x7 = { 0, 256, 755, 511, 0, 100, 0, 1, -1 };
struct X7 x7a;

void check_X7( struct X7 x7 )
{
    if( x7.f0 != 0   )  fail(__LINE__);
    if( x7.f1 != -256 ) fail(__LINE__);
    if( x7.f2 != 755 )  fail(__LINE__);
    if( x7.f3 != -1 )   fail(__LINE__);
    if( x7.f4 != 0   )  fail(__LINE__);
    if( x7.f5 != 100 )  fail(__LINE__);
    if( x7.f6 != 0   )  fail(__LINE__);
    if( x7.f7 != -1  )  fail(__LINE__);
    if( x7.m != -1   )  fail(__LINE__);
};

void set_X7a( void )
{
    x7a.f0 = 0;
    x7a.f1 = 256;
    x7a.f2 = 755;
    x7a.f3 = 511;
    x7a.f4 = 0;
    x7a.f5 = 100;
    x7a.f6 = 0;
    x7a.f7 = 1;
    x7a.m = -1;
}

void check_X7a( void )
{
    set_X7a();
    if( x7a.f0 != 0   )  fail(__LINE__);
    if( x7a.f1 != -256 ) fail(__LINE__);
    if( x7a.f2 != 755 )  fail(__LINE__);
    if( x7a.f3 != -1 )   fail(__LINE__);
    if( x7a.f4 != 0   )  fail(__LINE__);
    if( x7a.f5 != 100 )  fail(__LINE__);
    if( x7a.f6 != 0   )  fail(__LINE__);
    if( x7a.f7 != -1  )  fail(__LINE__);
    if( x7a.m != -1   )  fail(__LINE__);
};

int main( void )
{
    check_X1( x1 );
    check_X1a();
    check_X2( x2 );
    check_X3( x3 );
    check_X4( x4 );
    check_X5( x5 );
    check_X6( x6 );
    check_X7( x7 );
    check_X7a();
    _PASS;
}
