#include "fail.h"

int do_something;

struct C {
    int c1,c2;
    C() : c1(-__LINE__), c2(-__LINE__) {
    }
    C(C const &s) : c1(s.c1), c2(s.c2) {
    }
};
struct D {
    int d1,d2;
    ~D()
    { ++do_something; }
};
struct CD : C, D {
    int cd1,cd2;
};

extern "C" {
    // call to 'f1' pushes all structures
    // function 'f1' accesses structures by reference
    int f1( C x, D y, CD z )
    {
	if( x.c1 != 1 ) fail(__LINE__);
	if( x.c2 != 2 ) fail(__LINE__);
	if( y.d1 != -1 ) fail(__LINE__);
	if( y.d2 != -2 ) fail(__LINE__);
	if( z.c1 != -1 ) fail(__LINE__);
	if( z.c2 != -2 ) fail(__LINE__);
	if( z.d1 != 1 ) fail(__LINE__);
	if( z.d2 != 2 ) fail(__LINE__);
	if( z.cd1 != -1 ) fail(__LINE__);
	if( z.cd2 != -4 ) fail(__LINE__);
	return x.c1 + y.d1 + z.cd1;
    }
};

C x;
D y;
CD z;

void test() {
    if( f1( x,y,z ) != -1 ) fail(__LINE__);
}

int main() {
    x.c1 = 1;
    x.c2 = 2;
    y.d1 = -1;
    y.d2 = -2;
    z.c1 = -1;
    z.c2 = -2;
    z.d1 = 1;
    z.d2 = 2;
    z.cd1 = -1;
    z.cd2 = -4;
    test();
    _PASS;
}
