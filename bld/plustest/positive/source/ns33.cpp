#include "fail.h"

namespace ns {
template< class T >
struct A {
    int f();
    int g();
};

template< class T >
int A<T>::f() {
    return 1;
}
}

template< class T >
int ns::A<T>::g() {
    return 1;
}

int main()
{
    ns::A<int> a;

    if( ! a.f() ) fail(__LINE__);
    if( ! a.g() ) fail(__LINE__);

    _PASS;
}
