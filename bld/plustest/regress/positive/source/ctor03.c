#include "fail.h"

struct S {
    int a,b;
    S( int r = 0, int i = 0 ) : a(r), b(i) {}
};

struct T : S {
};

int main()
{
    T x;
    if( x.a != 0 || x.b != 0 ) fail(__LINE__);
    _PASS;
}
