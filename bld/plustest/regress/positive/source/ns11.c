#include "fail.h"

#ifdef __WATCOM_NAMESPACE__
namespace D {
    int d1 = -1;
    int f(char x) {
	return x + 1;
    }
}
using namespace D;

int d1 = 1;

namespace E {
    int e = 'e';
    void f(int);
}

namespace D {       // namespace extension
    int d2 = 2;
    using namespace E;
    void f(int);
}

void f() {
    if( ::d1 != 1 ) _fail;
    if( D::d1 != -1 ) _fail;
    if( d2 != 2 ) _fail;
    if( e != 'e' ) _fail;
    if( f('5') != '6' ) _fail;
}

int main() {
    f();
    _PASS;
}
#else
ALWAYS_PASS
#endif
