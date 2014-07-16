#include "fail.h"

typedef unsigned char base;

struct X1 {
    base f0 : 1;
    base f1 : 1;
    base f2 : 1;
    base f3 : 1;
    base f4 : 1;
    base f5 : 1;
    base f6 : 1;
    base f7 : 1;
    int m;
};

X1 x1 = { 0, 1, 0, 1, 0, 1, 0, 1, -1 };

void check_X1( X1 const &x1 ) {
    if( x1.f0 != 0 ) fail(__LINE__);
    if( x1.f1 != 1 ) fail(__LINE__);
    if( x1.f2 != 0 ) fail(__LINE__);
    if( x1.f3 != 1 ) fail(__LINE__);
    if( x1.f4 != 0 ) fail(__LINE__);
    if( x1.f5 != 1 ) fail(__LINE__);
    if( x1.f6 != 0 ) fail(__LINE__);
    if( x1.f7 != 1 ) fail(__LINE__);
    if( x1.m != -1 ) fail(__LINE__);
};

struct X2 {
    base f0 : 1;
    base f1 : 1;
    base f2 : 1;
    base f3 : 1;
    base f4 : 1;
    base f5 : 1;
    base f6 : 1;
    base f7 : 1;
    base f8 : 1;
    int m;
};

X2 x2 = { 0, 1, 0, 1, 0, 1, 0, 1, 1, -1 };

void check_X2( X2 const &x2 ) {
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

X3 x3 = { 0, 1, 0, 1, 0, 1, 0, 1, 1, -1 };

void check_X3( X3 const &x3 ) {
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

X4 x4 = { 0, 1, 0, 1, -1, 0, 1, 0, 1, -1, 1 };

void check_X4( X4 const &x4 ) {
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
    X1 x1;
    X2 x2;
    X3 x3;
    X4 x4;
};

X5 x5 = { 0, 1, 0, 1, -1, 0, 1, 0, 1, -1, 1,
{ 0, 1, 0, 1, 0, 1, 0, 1, -1 },
{ 0, 1, 0, 1, 0, 1, 0, 1, 1, -1 },
{ 0, 1, 0, 1, 0, 1, 0, 1, 1, -1 },
{ 0, 1, 0, 1, -1, 0, 1, 0, 1, -1, 1 },
};

void check_X5( X5 const &x5 ) {
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

int main()
{
    check_X1( x1 );
    check_X2( x2 );
    check_X3( x3 );
    check_X4( x4 );
    check_X5( x5 );
    _PASS;
}
