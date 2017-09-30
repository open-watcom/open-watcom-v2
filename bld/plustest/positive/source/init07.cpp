#include "fail.h"

struct S {
    S *p;
    int i;
    S(int x):p(0),i(x) {}
};

const class S &r = 0;

int main()
{
    if( r.p != 0 ) fail(__LINE__);
    if( r.i != 0 ) fail(__LINE__);
    _PASS;
}
