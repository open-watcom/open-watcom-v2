// see C++98 6.6.3 (3)
#include "fail.h"

unsigned ctr = 0;

int f1(unsigned i) {
    ctr += i;
    return ctr;
}

void g1() {
    return (const volatile void) f1(1);
}

void f2(unsigned i) {
    ctr += i;
}

void g2() {
    return f2(2);
}

int main() {
    g1();
    if( ctr != 1 ) fail( __LINE__ );

    g2();
    if( ctr != 3 ) fail( __LINE__ );

    _PASS;
}
