#include "fail.h"

#if __WATCOM_REVISION >= 8
struct S {
    char c;
    char S::* c_n;
    char const S::* c_c;
    char volatile S::* c_v;
    char const volatile S::* c_cv;
    S() : c_n(&S::c), c_c(&S::c), c_v(&S::c), c_cv(&S::c) {
    }
    char const S::* foo() {
	return &S::c;
    }
    char const S::* bar(int b) {
	return b ? 0 : &S::c;
    }
};
#endif

struct C {
    int x;
    C(int x) : x(x) {
    }
    C( C const volatile &s ) : x(s.x) {
    }
    int f() {
	return x;
    }
    int f() const {
	return x + 1;
    }
    int f() volatile {
	return x - 1;
    }
};
const C fc() {
    C x('c');
    return x;
};
volatile C fv() {
    C x('v');
    return x;
};

int main() {
    if( fc().f() != 'd' ) fail(__LINE__);
    if( fv().f() != 'u' ) fail(__LINE__);
    _PASS;
}
