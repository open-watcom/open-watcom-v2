// -d2 causes infinite loop in compiler
#include "fail.h"

class A {
public:
    virtual ~A() {}
    int a;
};

class B { int b; };

#ifdef __WATCOM_NAMESPACE__
namespace N {
    class C {int c;};

    typedef B T;

    class D {
    public:    
        virtual ~D() {}
	int d;
    };
    
    class E : public D, public A {
    public:
        E() {}
        B* getB() {
            return 0;
        }
        C* getC() {
            return 0;
        }
	int e;
    };
}
#endif

int main() {
    _PASS;
}
