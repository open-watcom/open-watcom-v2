#include "link14.h"

struct X : Scanner {
    ~X();
    int m;
};

// typesig needs dtor
X::~X() {
    ack+=__LINE__;
}

extern void foo();
void bar() {
    { X a;
	a.m = __LINE__;
	X *p = new X[10];
	delete [] p;
    }
    foo();
}
