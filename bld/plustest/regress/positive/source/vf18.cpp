#include "fail.h"

#if __WATCOM_REVISION__ >= 8

#pragma inline_depth(0)
struct B {
    int b;
    int c;
    virtual int foo() {
	return 0;
    }
    B() : b( dispatch() ) {
	c = dispatch();
    }
    virtual int dispatch() {
	return foo();
    }
};
struct D : B {
    int x;
    int y;
    D() : x(dispatch()) {
	y = dispatch();
    }
    virtual int foo() {
	return 1;
    }
};

int main() {
    D x;
    if( x.x != x.y ) _fail;
    if( x.x != 1 ) _fail;
    _PASS;
}
#else
ALWAYS_PASS
#endif
