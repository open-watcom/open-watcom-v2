#include "fail.h"

struct B1 {
    B1( int x = -1, int y = -2 ) : a(x), b(y) {}
    int a,b;
};
struct B2 {
    B2( int x = -3, int y = -4 ) : c(x), d(y) {}
    int c,d;
};
struct D : B1, B2 {
};
struct E : B1, B2 {
    E() : B1(1,2), B2(3,4) {}
};
struct X : D, E {
};

X dummy;

int main()
{
    D *pd = &dummy;
    E *pe = &dummy;

    if( pd->a != -1 || pd->b != -2 || pd->c != -3 || pd->d != -4 ) {
	fail(__LINE__);
    }
    if( pe->a != 1 || pe->b != 2 || pe->c != 3 || pe->d != 4 ) {
	fail(__LINE__);
    }
    _PASS;
}
