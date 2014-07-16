#include "fail.h"

struct S {
    int v;
    S() : v('s') {
    }
    S( S const & ) : v('s') {
    }
    ~S() {
	v = -'s';
    }
};

void bar( const S & t = S() )
{
    if( t.v != 's' ) fail(__LINE__);
}

int main()
{
    bar();
    bar( S() );
    _PASS;
}
