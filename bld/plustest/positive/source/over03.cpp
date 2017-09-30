#include "fail.h"
// more overloading
//

// all of these functions should return 150

enum A {
    c1 = 120
};

const int c2 = 120;

unsigned long b1()
{
    double a = 1.25;
    return a * c1;
}

unsigned long b2()
{
    double a = 1.25;
    return c1 * a;
}

unsigned long b3()
{
    float a = 1.25;
    return a * c1;
}

unsigned long b4()
{
    float a = 1.25;
    return c1 * a;
}

unsigned long ok()
{
    double a = 1.25;
    return a * c2;
}

void Err( const char* text )
{
    printf( "FAILED(over02): %s\n", text );
    fail(__LINE__);
}

struct T
{   int t;
    T( int tv ) : t(tv) {}
    operator long() const { return t; }
};

#define test150(fun) if( 150 != fun() ) Err( "150 != " #fun "()" );

int main()
{
    test150( b1 );
    test150( b2 );
    test150( b3 );
    test150( b4 );
    test150( ok );

    T t(67);
    if( t != 67 ) Err( "T t(67) != 67" );
    T t1 = t + t;
    if( t1 != 67*2 ) Err( "t1 != t + t" );

    _PASS;
}
