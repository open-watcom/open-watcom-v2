#include "fail.h"

template <class T>
struct A{
    T operator[](int i){ return i+x;}
    T x;
};

int f( A<int>& a )
{
    int r = a[3];
    r += a.operator[] (4);
    return r;
}

int g( A<int>* a )
{
    int r = (*a)[6];
    r += a->operator[]( 7 );
    return r;
}

int main () {
    A< int > a;
    a.x = 8;
    if( f(a) != 23 )  fail( __LINE__ );
    a.x = 1;
    if( g(&a) !=  15 )fail( __LINE__ );
    
    _PASS;
}

