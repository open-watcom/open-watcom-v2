// 14.1p13: "The scope of a template-parameter extends from its point of
// declaration until the end of its template. In particular, a template-
// parameter can be used in the declaration of subsequent template-
// parameters and their default arguments."

#include "fail.h"

template< class T, class U = T>
struct A {
    T member1;
    U member2;
};

template< class T, int n = sizeof( T ) >
struct B {
    static const int val = n;
};

int main()
{
    A<int> a;
    if( sizeof( a.member1 ) != sizeof( a.member2 ) ) fail(__LINE__);

    B<int> b;
    if( sizeof( int ) != b.val ) fail(__LINE__);

    _PASS;
}
