#include "fail.h"

int ctors;

struct S {
    int a;
    ~S();
    S(int=0);
    S(S const &);
};

S::S( int x ) : a(x) {
    ++ctors;
}

S::~S() {
    a = -1;
    --ctors;
    if( ctors < 0 ) fail(__LINE__);
}

S::S( S const &s ) : a(s.a) {
    ++ctors;
}

S do_add( S const &x, S const &y )
{
    return( S( x.a + y.a ) );
}

S add( S const &x, S const &y )
{
    return do_add( x, y );
}

int main()
{
    {
	S x( add( S(2), S(3) ) );
	if( x.a != 5 ) fail(__LINE__);
	S y( add( S(4), S(5) ) );
	if( y.a != 9 ) fail(__LINE__);
    }
    if( ctors != 0 ) fail(__LINE__);
    _PASS;
}
