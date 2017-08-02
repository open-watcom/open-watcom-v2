#include <limits.h>
#include "fail.h"

#ifdef __WATCOM_INT64__

class C : public _CD {
    public:
    __int64 a;
    C();
    ~C();
};

static C x;

__int64 doo( __int64 x ) {
    static C y;
    __int64 z = y.a;
    y.a = x;
    return z;
}

C::C() {
    a = -LONG_MIN-34;
}

C::~C() {
    a = LONG_MAX+34;
}

#endif
