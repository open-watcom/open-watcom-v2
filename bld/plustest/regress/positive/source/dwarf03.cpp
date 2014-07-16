// -d2 causes infinite loop in compiler
#include "fail.h"

struct B {
    int b;
};

#ifdef __WATCOM_NAMESPACE__
namespace N {
    typedef B T;

    T x;
};
#endif

int main() {
#ifdef __WATCOM_NAMESPACE__
    N::T a;
    a.b++;
#endif
    _PASS;
}
