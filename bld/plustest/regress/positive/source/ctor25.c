#include "fail.h"

// 96/04/12 -- J.W.Welch    -- B(x) is direct inititialization

#if __WATCOM_REVISION__ >= 8

struct B {
    int x;
    B( int x = 0, int = 0 ) : x(x) {
    }
    operator int() { return x; }
};

struct F {
    operator int() { return 1; }
    operator B() { return 0; }
};

struct Q {
    B x;
    Q( F f ) : x(f) {
        if( !x ) fail(__LINE__);
    }
};

int main() {
    F x;
    Q y(x);
    if( !B(x) ) fail(__LINE__);  // direct init
    if( !B(x,1) ) fail(__LINE__);
    _PASS;
}
#else

ALWAYS_PASS

#endif
