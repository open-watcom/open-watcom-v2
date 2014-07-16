#include "fail.h"

template<int n>
struct A {
    static const int res = A<n - 1>::res + n;
};

template<>
struct A<0> {
    static const int res = 0;
};

template<>
struct A<-1> {
    static const int res = -1;
};


template<class T1, class T2>
struct B {
    static const int val = 1;
};

template<>
struct B<int, long> {
    static const int val = 2;
};

template<>
struct B<char, char> {
    static const int val = 3;
};


int main() {
    if( A<4>::res != 10) fail(__LINE__);
    if( A<3>::res != 6) fail(__LINE__);
    if( A<2>::res != 3) fail(__LINE__);
    if( A<1>::res != 1) fail(__LINE__);
    if( A<0>::res != 0) fail(__LINE__);
    if( A<-1>::res != -1) fail(__LINE__);

    if( B<int, int>::val != 1) fail(__LINE__);
    if( B<int, long>::val != 2) fail(__LINE__);
    if( B<long, int>::val != 1) fail(__LINE__);
    if( B<long, long>::val != 1) fail(__LINE__);
    if( B<char, char>::val != 3) fail(__LINE__);

    _PASS;
}
