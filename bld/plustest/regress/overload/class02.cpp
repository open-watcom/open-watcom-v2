#include "dump.h"


class A { public: int a; };
class B : public A { public: int b; };
class C : public B { public: int c; };

void f( A * ) GOOD;
void f( B * ) GOOD;
void f( C * ) GOOD;

void g( A *a1, B *b1, C *c1 )
{
    f( a1 );
    f( b1 );
    f( c1 );
}
int main( void ) {
    g( 0, 0 ,0 );
    CHECK_GOOD( 27 );
    return errors != 0;
}
