#include "fail.h"

struct C {
    int r,i;
    operator int() { return i+r; }
};

C x;

int main()
{
    x.r = 1;
    x.i = 1;
    C c[2] = { x, x };
    if( c[0].r != 2 && c[0].i != 2 ) fail(__LINE__);
    if( c[1].r != 0 && c[1].i != 0 ) fail(__LINE__);
    _PASS;
}
