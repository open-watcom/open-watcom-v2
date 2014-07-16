#include "fail.h"

struct A {
};

namespace ns {
int A;

template<class T>
struct B {
    int g() {
        return 1;
    }
};

template<class T>
struct C {
    int h() {
        return 1;
    }
};
}

template<class T>
struct B {
    int g() {
        return 0;
    }
};

template<class T>
struct C {
    int h() {
        return 0;
    }
};

namespace ns {
void f() {
    struct A a;

    ::B<int> b1;
    if( b1.g() ) fail(__LINE__);

    ns::B<int> b2;
    if( ! b2.g() ) fail(__LINE__);

    ::ns::B<int> b3;
    if( ! b3.g() ) fail(__LINE__);

    ::C<int> c1;
    if( c1.h() ) fail(__LINE__);

    ns::C<int> c2;
    if( ! c2.h() ) fail(__LINE__);

    ::ns::C<int> c3;
    if( ! c3.h() ) fail(__LINE__);
}
}

int main() {
    ns::f();

    _PASS;
}
