#include "fail.h"

int weird_ellipse( int const &i, ... )
{
    return( i + i );
}

int main()
{
    int n = 10;

    if( weird_ellipse( n, &n ) != 20 ) fail(__LINE__);
    if( weird_ellipse( 10, &n ) != 20 ) fail(__LINE__);
    _PASS;
}
