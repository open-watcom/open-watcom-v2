#include "fail.h"
#include <new>

struct A {
    virtual ~A() {
        ctr++;
    }

    static int ctr;
};

int A::ctr = 0;


struct B : A {
    ~B() {
        ctr++;
    }

    static int ctr;
};

int B::ctr = 0;


int main()
{
    int buf[sizeof(B) / sizeof(int) + 1];
    A *a;

    a = new(&buf) B();
    A::ctr = B::ctr = 0;
    a->~A();
    if( ( A::ctr != 1 ) || ( B::ctr != 1 ) ) fail( __LINE__ );

    a = new(&buf) B();
    A::ctr = B::ctr = 0;
    a->A::~A();
    if( ( A::ctr != 1 ) || ( B::ctr != 0 ) ) fail( __LINE__ );


    typedef int I;

    I *ip = new I;
    ip->~I();
    ip->I::~I();
    delete ip;

    I i = 0;
    i.~I();
    i.I::~I();


    _PASS;
}
