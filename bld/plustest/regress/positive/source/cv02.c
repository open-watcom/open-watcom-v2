#include "fail.h"

struct S {
    int x;
    S( int x ) : x(x) {
    }
    operator const int &() { return x; }
};

S x(34);

int main() {
    const int &ci = x;
    if( ci != 34 ) fail(__LINE__);
#if __WATCOM_REVISION__ >= 8
    volatile const int &cvi1 = x;
    if( cvi1 != 34 ) fail(__LINE__);
#endif
    volatile const int &cvi2 = ci;
    if( cvi2 != 34 ) fail(__LINE__);
    _PASS;
}
