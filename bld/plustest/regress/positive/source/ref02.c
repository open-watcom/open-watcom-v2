#include "fail.h"

int i( int x )
{
    return -( x + 1 );
}

void foo( int (&x)( int ) )
{
    int (&f)( int ) = i;

    if( f(1) != -2 || x(2) != -3 ) fail(__LINE__);
}

int (&inc_neg)( int ) = i;

int (& ret_ref() )( int )
{
    return i;
}

int main()
{
    foo(i);
    foo(inc_neg);
    foo(ret_ref());
    _PASS;
}
