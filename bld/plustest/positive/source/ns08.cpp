#include "fail.h"

#ifdef __WATCOM_NAMESPACE__
namespace Outer {
    int i;
    namespace Inner {
	int f() { return i++; } // Outer::i
	int i;
	int g() { return i++; } // Inner::i
    }
}

int main() {
    using namespace Outer::Inner;
    if( f() != 0 ) _fail;
    if( f() != 1 ) _fail;
    if( g() != 0 ) _fail;
    if( g() != 1 ) _fail;
    _PASS;
}
#else
ALWAYS_PASS
#endif
