#include "fail.h"

class A {
    friend int ::foo( A * );
    int x;
public:
    A( int y ) : x(y) {}
};

class B;
int foo( B * );
class B {
    friend int ::foo( B * );
    int y;
public:
    B( int y ) : y(y) {}
};

int foo( A *p )
{
    return p->x;
}

int foo( B *p )
{
    return p->y;
}

int main() {
    A x(1);
    B y(2);

    if( foo( &x ) != 1 ) fail(__LINE__);
    if( foo( &y ) != 2 ) fail(__LINE__);
    _PASS;
}
