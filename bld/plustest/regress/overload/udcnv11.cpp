#include "dump.h"
struct B {
    B() {}
    B( const B & ) {};
};

struct D : public B {
    D( D const & );
};
B b1;

struct X {
    operator D();
    operator B(){GOOD; return b1; };
};

X x;

int main()
{
#if __WATCOM_REVISION__ >= 8
    B b = x;
#else
    FORCE_GOOD(14);
#endif
    CHECK_GOOD(14);
    return errors != 0;
}
