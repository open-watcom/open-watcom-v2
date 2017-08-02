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
        int b() {
            return x++;
        }
    };
    namespace {
        int c() {
            return x++;
        }
    };
};

int main() {
    if( A::a() != 0 ) fail(__LINE__);
    if( A::b() != 1 ) fail(__LINE__);
    if( A::c() != 2 ) fail(__LINE__);
    if( A::a() != 3 ) fail(__LINE__);
    if( A::b() != 4 ) fail(__LINE__);
    if( A::c() != 5 ) fail(__LINE__);
    _PASS;
}
#else
ALWAYS_PASS
#endif
