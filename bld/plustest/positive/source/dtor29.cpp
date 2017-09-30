#include "fail.h"

#pragma inline_depth(1)

enum E { E1, E2, E3 };

void die() {
    throw E2;
}

int x;

struct B {
    int c;
    int once;
    B() : c(0), once(0) {
    }
    ~B() {
	if( once ) {
	    fail(__LINE__);
	}
	++once;
	if( c != 0 ) {
	    die();
	}
    }
};

struct D {
    ~D() {
	x += __LINE__;
    }
};

struct L : D, B {
    void a() {
	++c;
    }
};

int main() {
    try {
	L x;
	x.a();
    } catch( E x ) {
	if( x != E2 ) fail(__LINE__);
    } catch( ... ) {
	fail(__LINE__);
    }
    _PASS;
}
