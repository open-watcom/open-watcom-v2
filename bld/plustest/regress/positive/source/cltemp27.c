#include "fail.h"

struct A
{
    int m;
};

template< class T >
struct B
{
    struct C
    {
        int m;
    };

    template< class U >
    struct D
    {
        int m;
    };

    int m;
};

int main()
{
    int A::* am;
    int B< int >::* bm;
    int B< int >::C::* cm;
    int B< int >::D< int >::* dm;

    am = &A::m;
    bm = &B< int >::m;
    cm = &B< int >::C::m;
    dm = &B< int >::D< int >::m;


    int A::* *ap;
    int B< int >::* *bp;
    int B< int >::C::* *cp;
    int B< int >::D< int >::* *dp;

    ap = new int A::*;
    *ap = am;
    delete ap;

    bp = new int B< int >::*;
    *bp = bm;
    delete bp;

    cp = new int B< int >::C::*;
    *cp = cm;
    delete cp;

    dp = new int B< int >::D< int >::*;
    *dp = dm;
    delete dp;


    _PASS;
}
