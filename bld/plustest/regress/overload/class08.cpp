#include "dump.h"


class A { public: int a; };
class B : public A { public: int b; };
class C : public B { public: int c; };

void f( C * ) BAD;
void f( void * ) GOOD;

void g( B *pb )
{
    f( pb );	// should pick f( void * );
}
int main( void ) {
    g( 0 );
    CHECK_GOOD( 9 );
    return errors != 0;
}
