#include "fail.h"

struct S {
    int v;
    S() : v(0) {}
    S( S const &s ) : v(s.v) {}
};

S test;

void changeTemps(S x, S y)
{
    if( x.v != 0 ) fail(__LINE__);
    if( y.v != 0 ) fail(__LINE__);
    x.v += 2;
    if( x.v != 2 ) fail(__LINE__);
    if( y.v != 0 ) fail(__LINE__);
    y.v += 2;
    if( x.v != 2 ) fail(__LINE__);
    if( y.v != 2 ) fail(__LINE__);
}

S &refTest()
{
    return test;
}

int main()
{
    if( test.v != 0 ) fail(__LINE__);
    changeTemps( refTest(), refTest() );
    if( test.v != 0 ) fail(__LINE__);
    _PASS;
}
