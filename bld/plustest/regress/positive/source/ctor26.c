#include "fail.h"

enum E {
    A,B,C
};

struct __pascal S {
    E x;
    S( E x ) : x(x) {
    }
};

S const foo()
{
    return S( B );
}

int main() {
    S x = foo();
    if( x.x != B ) fail(__LINE__);
    _PASS;
}
