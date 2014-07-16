#include "fail.h"

class Base {
    public:
        Base() {}
        virtual ~Base() {}
};

class A : public Base {
    public:
        A() {}
        ~A() {}

        virtual int OverriddenFunc( Base * ) = 0;
};

typedef int (A::*Callback)( Base * );

class Other : public Base {
    public:
        Other() {}
        ~Other() {}

        void SetCallback( Callback func )
        {
            _callback = func;
        }

    private:
        Callback        _callback;
};

class B : public A {
    public:
        B() {}
        ~B() {}

        void Register();

    private:
        Other           _other;
};

class C : public B {
    public:
        C() {}
        ~C() {}

        virtual int OverriddenFunc( Base * ) { return 0; }
};


void B::Register()
/****************/
{
    _other.SetCallback( (Callback)OverriddenFunc );
//  _other.SetCallback( (Callback)&A::OverriddenFunc );
}

int main() {
    C   c;

    c.Register();
    _PASS;
}
