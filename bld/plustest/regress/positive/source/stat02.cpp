#include "fail.h"

struct A {
    static const int val1 = 0;
    static const int val2 = val1 + 1;
    static const int val3 = val1 + val2;
};

template< class T >
struct B {
    static const T val1 = 0;
    static const T val2 = val1 + 1;
    static const T val3 = val1 + val2;
};

int main() {
    A a;
    B< int > b;

    if( A::val2 != A::val3 ) fail(__LINE__);
    if( a.val2 != a.val3 ) fail(__LINE__);

    if( B< int >::val2 != B< int >::val3 ) fail(__LINE__);
    if( b.val2 != b.val3 ) fail(__LINE__);

    _PASS;
}
