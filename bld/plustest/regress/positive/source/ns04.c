#include "fail.h"

#ifdef __WATCOM_NAMESPACE__
namespace A {
    namespace {
	int x;
	int a() {
	    return x++;
	}
    };
    namespace {
	int x;
	int b() {
	    return x++;
	}
    };
    namespace {
	int x;
	int c() {
	    return x++;
	}
    };
};

int main() {
    if( A::a() != 0 ) fail(__LINE__);
    if( A::b() != 0 ) fail(__LINE__);
    if( A::c() != 0 ) fail(__LINE__);
    if( A::a() != 1 ) fail(__LINE__);
    if( A::b() != 1 ) fail(__LINE__);
    if( A::c() != 1 ) fail(__LINE__);
    _PASS;
}
#else
ALWAYS_PASS
#endif
