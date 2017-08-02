#include "fail.h"

enum E { A, B, C };

int q = -23;

typedef int *P;

struct S {
    int a,b,c;
    S( int v ) : a(v), b(v+1), c(v-1) {}
};

typedef int F( int, int );

typedef int S::* MP;

int f( int x, int y )
{
    return x + y;
}

void def1( const long &v = 1 )
{
    if( v != 1 ) fail(__LINE__);
}

void def2( const float &v = 2 )
{
    if( v != 2 ) fail(__LINE__);
}

void def3( const double &v = 3 )
{
    if( v != 3 ) fail(__LINE__);
}

void def4( const E &v = B )
{
    if( v != B ) fail(__LINE__);
}

void def5( const P &v = &q )
{
    if( *v != -23 ) fail(__LINE__);
}

void def6( const S &v = S(100) )
{
    if( v.a != 100 ) fail(__LINE__);
    if( v.b != 101 ) fail(__LINE__);
    if( v.c != 99 ) fail(__LINE__);
}

void def7( const F &v = f )
{
    if( v(1,2) != 3 ) fail(__LINE__);
    if( v(3,4) != 7 ) fail(__LINE__);
}

void def8( const MP &v = &S::b )
{
    S x(-1);
    S y(-5);

    if( (x.*v) != 0 ) fail(__LINE__);
    if( (y.*v) != -4 ) fail(__LINE__);
}


int main()
{
    def1();
    def2();
    def3();
    def4();
    def5();
    def6();
    def7();
    def8();
    _PASS;
}
