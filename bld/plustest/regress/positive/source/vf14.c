#include "fail.h"

struct B {
    B() {}
};
struct D : B {
    virtual B() { return __LINE__; };
};
struct E : D {
    virtual int B() { return __LINE__; };
};

int getB( D *p )
{
    return p->B();
}

int main() {
    E e;
    D d;
    int e_B = getB( &e );
    int d_B = getB( &d );
    if( e_B <= d_B ) fail(__LINE__);
    if(( e_B - d_B ) != 3 ) fail(__LINE__);
    _PASS;
}
