#include "fail.h"

struct S {
    int v;
    ~S();
    S( int v ) : v(v) {
    }
    int a() {
	return v + 1;
    }
    int a() const {
	return v;
    }
};
S fn_retS( int v ) {
    return S( v );
}
void useS( const S x )
{
    if( x.a() != 31 ) fail(__LINE__);
}
int main( void )
{
    useS( fn_retS( 31 ) );
    _PASS;
}

S::~S() {
    if( a() != 32 ) fail(__LINE__);
}
