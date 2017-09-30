#include "fail.h"

int g() {
    return 1;
}

int h() {
    return 0;
}


template <class T> struct A;

template <class T> struct A<T *>;

template< > struct A<long> { };


A<int *> *ptr;

typedef A<long> B;

template< > struct A<short> { };


template <class T>
struct A {
    int f() {
        return g();
    }
};

template <class T>
struct A<T *> {
    int f() {
        return h();
    }
};


struct C1
{ };

struct C2
{ };

template< class T >
struct C
{ };

template<>
struct C< C1 >
{
    static const int val = 1;
};

template<>
struct C< C2 >
{
    static const int val = 2;
};


int main()
{
    A<int *> a;
    if( a.f() != 0 ) fail( __LINE__ );

    ptr = &a;
    if( ptr->f() != 0 ) fail( __LINE__ );

    if( C< C1 >::val != 1 ) fail( __LINE__ );
    if( C< C2 >::val != 2 ) fail( __LINE__ );


    _PASS;
}
