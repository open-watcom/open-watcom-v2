#include "fail.h"

template< class T >
struct A {
    A( const char* , T t = T() )
    { }

    A( T t = T() )
    { }
};


int main()
{
    A<float> a1[] = { "1", "2" };
    A<float> a2[2];

    _PASS;
}
