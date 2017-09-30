#include "dump.h"
class A {
    public:
    int a;
    A() { a = 0; }
    A( int a1 ) { a = a1; }
};

A a( 1 );
A b = 2;
int main( void )
{
    DUMP( a );
    DUMP( b );
    return 0;
}
