#include "fail.h"

#pragma on (check_stack)

template <int n>
struct A{
    int d[n];
    A( ) { }
};

template <long long n>
struct B{
    int d[n];
    B( ) { }
};

int main() {
    A<100> a;
    B<18> b;    // shouldn't cause a stack overflow!
    _PASS;
}

