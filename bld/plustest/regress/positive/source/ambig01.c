#include "fail.h"

struct A {
    static void f();
    static int x;
};
void A::f() {x|=0x01;};

struct B : public virtual A {
    static void f();
    static int x;
};
void B::f() {x|=0x02;};

struct C : public virtual A, public B {
    void func();
};
void C::func() {
    f(); // ambiguous?
    x |= 0x04; // ambiguous?
}

int A::x;
int B::x;

int main()
{
    C dummy;

    dummy.func();
    if( A::x != 0 || B::x != 6 ) {
	fail(__LINE__);
    }
    _PASS;
}
