#include "fail.h"

char test;
struct A {
    int a;
    virtual void foo()
    {
	test = 'A';
    }
};

struct B {
    int b;
    virtual void foo()
    {
	test = 'B';
    }
};

struct C : A, B {
    int c;
};

C x;

int main( void )
{
    A *pa;
    B *pb;

    pa = &x;
    pb = &x;
    pa->foo();
    if( test != 'A' ) fail(__LINE__);
    pb->foo();
    if( test != 'B' ) fail(__LINE__);
    _PASS;
}
