#include "fail.h"

static int near1( int x, int y )
{
    return x | y;
}

static int far1( int x, int y )
{
    return x + y;
}

int (*f1)( int, int ) = far1;

int main()
{
    if( near1( 2, 4 ) != 6 ) fail(__LINE__);
    if( far1( 1, 8 ) != 9 ) fail(__LINE__);
    if( f1( 1, 4 ) != 5 ) fail(__LINE__);
    _PASS;
}
