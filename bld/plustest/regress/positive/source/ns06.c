#include "fail.h"

#ifdef __WATCOM_NAMESPACE__
namespace A {
    int i;
};

int i;

namespace TEST {
    using namespace A;
    void foo() {
	::i++;
	A::i++;
    }
};

using namespace TEST;

int main() {
    foo();
    if( A::i != 1 ) fail(__LINE__);
    if( ::i != 1 ) fail(__LINE__);
    _PASS;
}
#else
ALWAYS_PASS
#endif
