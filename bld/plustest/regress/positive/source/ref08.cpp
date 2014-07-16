#include "fail.h"

int foo( int x, const int &y )
{
    return x * y;
}

const int gg = -28;

int main()
{
    const int zz = 28;
    if( foo( 6, zz ) != (6*28) ) fail(__LINE__);
    if( foo( __LINE__, zz ) != (__LINE__*28) ) fail(__LINE__);
    if( foo( -6, zz ) != (-6*28) ) fail(__LINE__);
    if( foo( 6, gg ) != (6*-28) ) fail(__LINE__);
    if( foo( __LINE__, gg ) != (__LINE__*-28) ) fail(__LINE__);
    if( foo( -6, gg ) != (-6*-28) ) fail(__LINE__);
    _PASS;
}
