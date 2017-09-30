#include "fail.h"

namespace ns {
template<class T>
struct A {
};

struct B {
};
}

int main() {
    struct ::ns::A<int> a1;
    struct ns::A<long> a2;

    struct ::ns::B b1;
    struct ns::B b2;

    _PASS;
}
