#include "fail.h"

struct V {
    int x;
    int v;
    virtual void foo( void );
};

void V::foo( void )
{
    if( x != 0x5555 ) fail(__LINE__);
    x = 0x7777;
    if( v != 0x6666 ) fail(__LINE__);
    v = 0x8888;
}

struct R {
    int f1;
    int r;
};

struct M : R, V {
    int f2;
    int m;
};

struct P {
    int f3;
    int p;
};

struct B : P, virtual M {
    int f4;
    int b;
};

int main( void )
{
    static B x;
    B *p = &x;
    p->b = 0x1111;
    p->p = 0x2222;
    p->m = 0x3333;
    p->r = 0x4444;
    p->x = 0x5555;
    p->v = 0x6666;
    p->foo();
    _PASS;
}
