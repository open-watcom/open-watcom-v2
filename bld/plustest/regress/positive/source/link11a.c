#include "link11.h"
#include "fail.h"

#if __WATCOMC__ > 1060
extern template TC<1>;
#endif

TC <1> x;
TC <2> y;

#if __WATCOMC__ > 1060
extern template TC<2>;
#endif

int main() {
    if( x.sig() != 1 ) fail(__LINE__);
    if( y.sig() != 2 ) fail(__LINE__);
    _PASS;
}
