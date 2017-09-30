#include "fail.h"

template <class T>
    struct S {
	static int s;
	T a;
	int foo() {
	    return s;
	}
    };

typedef int A1[1];
S< A1 > v1;
int S<A1>::s = 1;

#if __WATCOM_REVISION__ >= 8

typedef int A10[10];
S< A10 > v10;
int S<A10>::s = 10;

#endif

int main() {
    if( v1.foo() != 1 ) fail(__LINE__);
#if __WATCOM_REVISION__ >= 8
    if( v10.foo() != 10 ) fail(__LINE__);
#endif
    _PASS;
}
