#include "fail.h"

template<class T>
struct B {
    int g() {
        return 0;
    }
};

namespace ns {
template<class T>
struct B {
    int g() {
        return 1;
    }
};
}

int main() {
    ns::B<int> b1;
    if( ! b1.g() ) fail(__LINE__);

    ::B<int> b2;
    if( b2.g() ) fail(__LINE__);

    _PASS;
}
