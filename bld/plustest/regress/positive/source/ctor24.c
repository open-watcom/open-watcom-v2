#include "fail.h"

// 96/04/12 -- J.W.Welch    -- B(x) is direct inititialization

#if __WATCOM_REVISION__ >= 8

#pragma inline_depth(0)

struct B {
    int x;
    B( int x = 0, int = 0 ) : x(x) {
    }
    operator int() { return x; }
};

struct F {
    operator int() { return 1; }

#if 0
    // ambiguous
    operator B() { return 0; }
#endif
};

int main() {
    F x;
    if( !B(x) ) fail(__LINE__); // direct init
    if( !B(x,1) ) fail(__LINE__);
    _PASS;
}
#else

ALWAYS_PASS

#endif
