#include "fail.h"
#include <stdio.h>
#include <stdlib.h>

int zz;

union U {
    int a;
    U(int x ) : a(x) {}
    ~U() { zz = a; }
};

void foo()
{
    U x(1);
}

int main()
{
    foo();
    if( zz == 0 ) fail(__LINE__);
    _PASS;
}
