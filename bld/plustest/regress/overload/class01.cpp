#include "dump.h"


class A { public: int a; };
class B { public: int b; };

void f( A ) GOOD;
void f( A * ) GOOD;
void f( B ) GOOD;
void f( B * ) GOOD;

void g( void )
{
    A a1;
    B b1;
    f( a1 );
    f( &a1 );
    f( b1 );
    f( &b1 );
}
int main( void ) {
    g();
    CHECK_GOOD( 34 );
    return errors != 0;
}
