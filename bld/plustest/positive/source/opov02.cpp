#include "fail.h"

struct S {
    S * operator ->();
    int s;
};
struct P {};
struct T : P, S {
};

int foo( T &r )
{
    return r->s;
}
S * S::operator ->()
{
    ++s;
    return this;
}

int main() {
    T x;
    x.s = 1;
    if( foo( x ) != 2 ) fail(__LINE__);
    x.s = -1;
    if( foo( x ) != 0 ) fail(__LINE__);
    _PASS;
}
