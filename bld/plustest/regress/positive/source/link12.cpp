#include "fail.h"

struct S {
    static int x;
    int y;
    S( int y ) : y(y) {
	++y;	// affects local 'y'
    }
    ~S() {
	--y;
    }
};

int foo( S *p ) {
    return p->y + 3;
}

int main() {
    {
	S q(45);

	if( foo( &q ) != 45+3 ) fail(__LINE__);
    }
    _PASS;
}
