#include "dump.h"


//           A
//          / \             //
//         B   C
//          \ /
//	     D

class A { public: int a; };
class B : virtual public A { public: int b; };
class C : virtual public A { public: int c; };
class D : public B, public C { public: int d; };

void f( A * ) GOOD;
void f( char ) BAD;

void g( D *pd )
{
    f( pd );
}
int main( void ) {
    g( 0 );
    CHECK_GOOD( 15 );
    return errors != 0;
}
