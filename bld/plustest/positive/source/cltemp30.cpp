#include "fail.h"

struct C{ int static const v = 789; };

template< typename T >
struct B{ typedef C base; };

template< typename T >
struct A : public B<T>::base { };


int main() {
    A<int> a0; if( a0.v != 789 ) fail( __LINE__ );

    _PASS;
}

