#include "fail.h"

namespace ns {
template< class T >
struct A {
};

struct B {
};
}

namespace ns2 {
using ns::A;
using ns::B;
}

namespace ns3 {
struct C {
};
}

using namespace ns3;

int main() {
    ns2::A<int> a1;
    ns2::A<long> a2;
    ns2::B b1;
    C c1;

    _PASS;
}
