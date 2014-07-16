#include "fail.h"

#ifdef __WATCOM_NAMESPACE__
int which;

namespace A {
    void f(int x) {
	if( x != 'a' ) _fail;
	which |= 1;
    }
}

using A::f;              // f is a synonym for A::f;
	       		// that is, for A::f(int).
namespace A {
    void f(char x ) {
	if( x != 'a' ) _fail;
	which |= 2;
    }
}

void foo() {
    which = 0;
    f('a');          // calls f(int),
    if( which != 1 ) _fail;
}                        // even though f(char) exists.

void bar() {
    using A::f;      // f is a synonym for A::f;
		   // that is, for A::f(int) and A::f(char).
    which = 0;
    f('a');          // calls f(char)
    if( which != 2 ) _fail;
}

int main() {
    foo();
    bar();
    _PASS;
}
#else
ALWAYS_PASS
#endif
