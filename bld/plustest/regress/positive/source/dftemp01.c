// 14.1p9: "A default template-argument may be specified for any kind of
// template-parameter (type, non-type, template)."

#include "fail.h"

template<class T = int>
struct A {
    T member;
};

template<int n = 1>
struct B {
    int member;
    B( ) : member(n) { }
};

// Need example using template template parameters when they are supported.

int main()
{
    A<> object1;
    // Is this reliable?
    if( sizeof( object1 ) != sizeof( int ) ) fail(__LINE__);

    B<> object2;
    if( object2.member != 1 ) fail(__LINE__);

    _PASS;
}
