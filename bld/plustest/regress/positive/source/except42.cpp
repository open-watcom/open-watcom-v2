#include "fail.h"

static int zz;

struct T : _CD {
    T();
    ~T();
};

T::T(){++zz;}
T::~T(){--zz;}

struct A : _CD {
    A();
    ~A();
};

A::A(){++zz;}
A::~A(){--zz;}

short foo( A *a, int b) {
    T v1;
    if( b == 1 ) {
	return 1;
    }
    delete a;
    return 0;
}

int main() {
    A *p = new A();
    if( foo( p, 1 ) != 1 ) _fail;
    // no delete should have happened
    if( zz != 1 ) _fail;
    if( foo( p, 0 ) != 0 ) _fail;
    // delete should have happened
    if( zz != 0 ) _fail;
    _PASS;
}
