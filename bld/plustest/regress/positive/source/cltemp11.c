#include "fail.h"

int n = 0;

template<class T, int n>
struct A {
    int f() {
        return n;
    }
    int g();
};

template<class T, int mm>
int A<T, mm>::g() {
    return mm - n;
}


template<class T>
struct A<T, 1> {
    int f() {
        return 0;
    }
    int g();
};

template<class T>
int A<T, 1>::g() {
    return 0;
}


template<class T, int m>
struct A<T *, m> {
    int f() {
        return -m;
    }
    int g();
};

template<class T, int mm>
int A<T *, mm>::g() {
    return -mm;
}


template<int m>
struct A<int *, m> {
    int f() {
        return -2 * m;
    }
    int g();
};

template<int mm>
int A<int *, mm>::g() {
    return -2 * mm;
}


template<>
struct A<int *, 42> {
    int f() {
        return 4242;
    }
    int g();
};

int A<int *, 42>::g() {
    return 4242;
}


int main() {
    A<char, 42> a1;
    if( a1.f() != 42) fail(__LINE__);
    n = 0;
    if( a1.g() != 42) fail(__LINE__);
    n = 12;
    if( a1.g() != 30) fail(__LINE__);
    n = 0;

    A<char, 1> a2;
    if( a2.f() != 0) fail(__LINE__);
    if( a2.g() != 0) fail(__LINE__);

    A<char *, 42> a3;
    if( a3.f() != -42) fail(__LINE__);
    if( a3.g() != -42) fail(__LINE__);

    A<int *, 2> a4;
    if( a4.f() != -4) fail(__LINE__);
    if( a4.g() != -4) fail(__LINE__);

    A<int *, 42> a5;
    if( a5.f() != 4242) fail(__LINE__);
    if( a5.g() != 4242) fail(__LINE__);


    _PASS;
}
