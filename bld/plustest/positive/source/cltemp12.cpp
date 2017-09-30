#include "fail.h"

template<int n, int m>
struct A {
    static const int res = 0;
};

template<int n>
struct A<n, n> {
    static const int res = 1;
};

template<int n>
struct A<1, n> {
    static const int res = 2;
};

template<>
struct A<1, 1> {
    static const int res = 3;
};


template<int n, int m>
struct B {
    static const int res = 0;
};

template<>
struct B<1, 1> {
    static const int res = 3;
};

template<int n>
struct B<1, n> {
    static const int res = 2;
};

template<int n>
struct B<n, n> {
    static const int res = 1;
};


template<class T, class U>
struct C {
    static const int res = 0;
};

template<class T>
struct C<T *, T> {
    static const int res = 2;
};

template<class T, class U>
struct C<const T *, U> {
    static const int res = 3;
};

template<class T, class U>
struct C<T *, U> {
    static const int res = 1;
};


int main() {
    if( A<0, 1>::res != 0 ) fail( __LINE__ );
    if( A<0, 0>::res != 1 ) fail( __LINE__ );
    if( A<1, 2>::res != 2 ) fail( __LINE__ );
    if( A<1, 1>::res != 3 ) fail( __LINE__ );

    if( B<0, 1>::res != 0 ) fail( __LINE__ );
    if( B<0, 0>::res != 1 ) fail( __LINE__ );
    if( B<1, 2>::res != 2 ) fail( __LINE__ );
    if( B<1, 1>::res != 3 ) fail( __LINE__ );

    if( C<char , char>::res != 0 ) fail( __LINE__ );
    if( C<char *, int>::res != 1 ) fail( __LINE__ );
    if( C<char *, char>::res != 2 ) fail( __LINE__ );
    if( C<const char *, int>::res != 3 ) fail( __LINE__ );
    if( C<const char *, char>::res != 3 ) fail( __LINE__ );

    _PASS;
}
