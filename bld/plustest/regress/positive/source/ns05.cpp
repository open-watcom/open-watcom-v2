#include "fail.h"

#ifdef __WATCOM_NAMESPACE__
namespace A {
    int x;
    int foo() {
	return A::x++;
    }
};
int foo() {
    return A::x++;
}

int main() {
    if( foo() != 0 ) fail(__LINE__);
    if( A::foo() != 1 ) fail(__LINE__);
    _PASS;
}
#else
ALWAYS_PASS
#endif
