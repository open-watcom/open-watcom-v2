#include "fail.h"

struct B : _CD {
    char *n;
    B(char*n) : n(n) {
    }
};

struct D : B {
    D(char*n) : B(n) {
    }
};

int watch;
struct W {
    int s;
    W( int s ) : s(s) {
    }
    ~W() {
	watch = s;
    }
};

static void xB() throw(B) {
    W x(1);
    throw D("xB");
}

static void xpB() throw(B*) {
    W x(2);
    static D d("xpB");
    throw &d;
}

int main() {
    if( watch != 0 ) fail(__LINE__);
    try {
	xB();
    } catch( B h ) {
	if( watch != 1 ) fail(__LINE__);
	if( strcmp( h.n, "xB" ) != 0 ) fail(__LINE__);
    } catch( ... ) {
	fail(__LINE__);
    }
    if( watch != 1 ) fail(__LINE__);
    watch = 0;
    try {
	xpB();
    } catch( B *h ) {
	if( watch != 2 ) fail(__LINE__);
	if( strcmp( h->n, "xpB" ) != 0 ) fail(__LINE__);
    } catch( ... ) {
	fail(__LINE__);
    }
    if( watch != 2 ) fail(__LINE__);
    _PASS;
}
