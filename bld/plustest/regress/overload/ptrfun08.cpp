#include "dump.h"

#if __WATCOM_REVISION__ >= 8
struct X {
    int f(int) {GOOD; return 0;};
    static int f(long) {GOOD; return 0;};
};

int (X::*p1)(int) = &X::f;	// OK
int    (*p2)(long) = &X::f;	// OK
int    (*p3)(long) = &(X::f);	// OK

// from [over.over]  (13.4)
#endif

int main()
{
#if __WATCOM_REVISION__ >= 8
    X x;

    (x.*p1)(0);
#else
    FORCE_GOOD(5);
#endif
    CHECK_GOOD(5);
#if __WATCOM_REVISION__ >= 8
    (*p2)(1);
#else
    FORCE_GOOD(6);
#endif
    CHECK_GOOD(5+6);
#if __WATCOM_REVISION__ >= 8
    (*p3)(1);
#else
    FORCE_GOOD(6);
#endif
    CHECK_GOOD(5+6+6);
    return errors != 0;
}
