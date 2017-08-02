#include "fail.h"
//this was reduced down from stlport - it was crashing compiler

template <class P>
struct A{
};

template <class Q, class R>
struct B
{
};

template <class S, class T>
struct B< S, A<T> > {
    typedef int K;
};

struct C
{
    typedef B< int , A<bool> >::K L;
    typedef B< bool, A<bool> >::K M;
};

template< class X >
struct D
{
    typedef typename B< int , A<bool> >::K L;
    typedef typename B< bool, A<bool> >::K M;
};

int main() {
    C c;
    D<int> d;
    _PASS;
}
