#include "fail.h"

namespace ns {
    namespace ns2 {
        struct A {
        };
    }

    namespace {
        struct B {
        };
    }

    struct C {
    };

    struct D;
    struct D {
    };
}

struct E {
    ~E();

    struct F {
        ~F();
    };
};

E::~E()
{ }

E::F::~F()
{ }


int main() {
    ::ns::ns2::A a1;
    ns::ns2::A a2;
    struct ::ns::ns2::A a3;
    struct ns::ns2::A a4;

    ::ns::B b1;
    ns::B b2;
    struct ::ns::B b3;
    struct ns::B b4;

    struct ::ns::C::C c3;
    struct ns::C::C c4;

    struct ::ns::D::D d3;
    struct ns::D::D d4;

    E e;
    E::F f;


    _PASS;
}
