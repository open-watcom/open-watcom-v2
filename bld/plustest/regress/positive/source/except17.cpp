#include "fail.h"

int p_count;

struct p {
    int x,y;
    p( int x, int y ) : x(x), y(y) { ++p_count;
	}
    p( p const &s ) : x(s.x), y(s.y) { ++p_count;
	}
    ~p() {--p_count;
	}
};

void foo() {
    p x(-3,-4);

    throw p(-1,-2);
}

int main()
{
    try {
	foo();
    } catch( p v ) {
	if( v.x != -1 ) fail(__LINE__);
	if( v.y != -2 ) fail(__LINE__);
    } catch( ... ) {
	fail(__LINE__);
    }
    if( p_count != 0 ) fail(__LINE__);
    try {
	throw p(-5,-6);
    } catch( p v ) {
	if( v.x != -5 ) fail(__LINE__);
	if( v.y != -6 ) fail(__LINE__);
    } catch( ... ) {
	fail(__LINE__);
    }
    if( p_count != 0 ) fail(__LINE__);
    _PASS;
}
