#include "fail.h"

int foo( const int &r = -1 ) {
    return r + 2;
}

const int &x = -2;
int sam( const int &r = x ) {
    return r + 3;
}

struct S;
S *p;
S const &qq = *p;
int ack( S const & = qq )
{
    return 1;
}

struct A { virtual void foo() = 0; };
struct D : A { void foo() {}; };
A const &rr = D();
int qwe( A const & = rr )
{
    return 1;
}

int bar()
{
    return sam() + foo() + ack() + qwe();
}

int main()
{
    if( bar() != 4 ) fail(__LINE__);
    _PASS;
}
