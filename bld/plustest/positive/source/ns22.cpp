#include "fail.h"

#ifdef __WATCOM_NAMESPACE__
int which;

void f() {
    which |= 1;
};

namespace A {
    void g() {
	which |= 2;
    };
}

namespace X {
    using ::f;   // global f
    using A::g;  // A's g
}

void h() {
    X::f();      // calls ::f
    X::g();      // calls A::g
}

int main() {
    h();
    if( which != 3 ) _fail;
    _PASS;
}
#else
ALWAYS_PASS
#endif
