#include "fail.h"

struct S {
    int a,b,c;
};

S foo( int v )
{
    S x;

    x.a = v+1;
    x.b = v+2;
    x.c = v+3;
    return x;
}

struct T {
    char a,b,c;
};

T foo( char c )
{
    T x;

    x.a = c+1;
    x.b = c+2;
    x.c = c+3;
    return x;
}

int main()
{
    if( foo( 0 ).b != 2 ) fail(__LINE__);
    if( foo( -3 ).c != 0 ) fail(__LINE__);
    if( foo( '0' ).b != '2' ) fail(__LINE__);
    if( foo( 'a' ).c != 'd' ) fail(__LINE__);
    _PASS;
}
