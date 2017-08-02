#include "fail.h"

#if __WATCOM_REVISION__ >= 8
#pragma inline_depth(0)

typedef unsigned char UBYTE;

struct B {
   B& operator= (B& c);
};

struct D : public B {
   UBYTE aByte;
   UBYTE flag1:1;
   UBYTE flag2:1;
};

struct S {
    int d;
    D m1;
    D m2;
    int e;
};

void test1( int f1, int f2 ) {
    S x;
    
    memset( &x, -3, sizeof( x ) );
    x.e = -1;
    x.d = -1;
    x.m1 = x.m2;
    x.m2 = x.m1;
    x.m1.aByte = 0;
    x.m1.flag1 = f1;
    x.m1.flag2 = f2;
    x.m2 = x.m1;
    if( x.d != -1 ) _fail;
    if( x.e != -1 ) _fail;
    if( x.m1.flag1 != f1 ) _fail;
    if( x.m1.flag2 != f2 ) _fail;
    if( x.m2.flag1 != f1 ) _fail;
    if( x.m2.flag2 != f2 ) _fail;
}
void test2( int f1, int f2 ) {
    S x;
    
    memset( &x, -3, sizeof( x ) );
    x.e = -1;
    x.d = -1;
    x.m1 = x.m2;
    x.m2 = x.m1;
    x.m2.aByte = 0;
    x.m2.flag1 = f1;
    x.m2.flag2 = f2;
    x.m1 = x.m2;
    if( x.d != -1 ) _fail;
    if( x.e != -1 ) _fail;
    if( x.m1.flag1 != f1 ) _fail;
    if( x.m1.flag2 != f2 ) _fail;
    if( x.m2.flag1 != f1 ) _fail;
    if( x.m2.flag2 != f2 ) _fail;
}

int main()
{
    test1( 0, 0 );
    test1( 0, 1 );
    test1( 1, 0 );
    test1( 1, 1 );
    test2( 0, 0 );
    test2( 0, 1 );
    test2( 1, 0 );
    test2( 1, 1 );
   _PASS;
}

B& B::operator= (B& c){
    return c;
}
#else
ALWAYS_PASS
#endif
