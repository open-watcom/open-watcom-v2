#include "fail.h"

void *c;

class X {
protected:
    void foo(char *p = 0 ) {
	if( p ) fail(__LINE__);
	if( this != c ) fail(__LINE__);
    }
};
class Y;
class Z {
    friend int main();
    void bar( Y * );
};
class Y : public X {
    friend class Z;
};
void Z::bar( Y *p ) {
    p->foo();
}

class A {
private:
    static void f() {
    }

public:
    class B {
    public:
        static void f() {
            A::f();
        }
    };
};

int main() {
    Y y;
    Z z;

    c = (X*) &y;
    z.bar( &y );

    A::B::f();

    _PASS;
}
