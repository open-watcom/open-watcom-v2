#include "fail.h"
#include <limits.h>

template <class T>
    int foo( T const &r ) {
	return r();
    }

struct S {
    S( int x ) : x(x) {
    }
    int operator()() const;
    int x;
};

int S::operator()() const {
    return x;
}

int rMAX() {
    return INT_MAX;
}

int main() {
    S x(__LINE__);
    if( foo( x ) != (__LINE__-1) ) fail(__LINE__);
#if __WATCOM_REVISION__ >= 8
    if( foo( rMAX ) != INT_MAX ) fail(__LINE__);
#endif
    _PASS;
}
