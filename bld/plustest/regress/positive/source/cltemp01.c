#include "fail.h"

template <int i>
struct Fib {
    enum { val = Fib<i-1>::val + Fib<i-2>::val };
};

template<>
struct Fib<0> {
    enum { val = 1 };
};

template<>
struct Fib<1> {
    enum { val = 1 };
};

int main()
{
    Fib<2> x2;
    Fib<3> x3;
    Fib<4> x4;
    Fib<5> x5;
    Fib<6> x6;
    Fib<7> x7;
    Fib<8> x8;
    Fib<9> x9;
    Fib<10> x10;

    if( x2.val != 2 ) fail(__LINE__);
    if( x3.val != 3 ) fail(__LINE__);
    if( x4.val != 5 ) fail(__LINE__);
    if( x5.val != 8 ) fail(__LINE__);
    if( x6.val != 13 ) fail(__LINE__);
    if( x7.val != 21 ) fail(__LINE__);
    if( x8.val != 34 ) fail(__LINE__);
    if( x9.val != 55 ) fail(__LINE__);
    if( x10.val != 89 ) fail(__LINE__);
    _PASS;
}
