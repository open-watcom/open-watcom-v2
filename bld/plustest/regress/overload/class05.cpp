#include "dump.h"

//             B
//            /
//       A   C
//        \ /
//         D

class A { public: int a; };
class B { public: int b; };
class C : public B { public: int c; };
class D : public A, public C { public: int d; };

void f( C * ) GOOD;
void f( B * ) BAD;

void g( D *pd )
{
    f( pd );	// should pick f( B * );
}
int main( void ) {
    g( 0 );
    CHECK_GOOD( 14 );
    return errors != 0;
}
