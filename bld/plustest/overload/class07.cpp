#include "dump.h"


class A { public: int a; };
class B : public A { public: int b; };
class C : public B { public: int c; };

void f( A * ) GOOD;
void f( void * ) BAD;

void g( C *pc )
{
    f( pc );	// should pick f( A * );
}
int main( void ) {
    g( 0 );
    CHECK_GOOD( 8 );
    return errors != 0;
}
