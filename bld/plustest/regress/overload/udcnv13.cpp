#include "dump.h"

struct X
{
    X(...) {GOOD}; // line 5
    X(X const &) { GOOD }; // line 6
};

int main()
{
#if __WATCOM_REVISION__ >= 8
    X x2 = 2;
#else
    FORCE_GOOD(5);
#endif
    CHECK_GOOD(5);
    return errors != 0;
}
