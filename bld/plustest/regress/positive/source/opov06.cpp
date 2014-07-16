#include "fail.h"

#ifdef __WATCOM_NAMESPACE__

namespace x {
    enum E { A };
    struct C { };
    int u = __LINE__;
    int operator ++( E & ) { return __LINE__; }
    int operator ++( E &, int ) { return __LINE__; }
    int operator ++( C & ) { return __LINE__; }
    int operator ++( C &, int ) { return __LINE__; }
};

void foo( x::E r ) {
    ++r;
    r++;
}
int main() {
    x::E e;
    x::C c;
    unsigned line = x::u;
    if( ++e != ++line ) _fail;
    if( e++ != ++line ) _fail;
    if( ++c != ++line ) _fail;
    if( c++ != ++line ) _fail;
    _PASS;
}
#else
ALWAYS_PASS
#endif
