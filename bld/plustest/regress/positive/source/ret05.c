#include "fail.h"

#pragma warning 665 10

struct S {
    int x;
};
struct T {
    T(){;}
    int x;
};

void foo( T & )
{
}
void foo( T const & )
{
    fail(__LINE__);
}
void foo( S & )
{
}
void foo( S const & )
{
    fail(__LINE__);
}

S ack(int)
{
    S v;

    return v;
}

T ack(char)
{
    return T();
}

int main()
{
    foo( ack(0) );
    foo( ack('0') );
    _PASS;
}
