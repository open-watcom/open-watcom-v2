#include "dump.h"


class A { public: int a; };
class B : public A { public: int b; };
class C : public B { public: int c; };

void f( A near * ) GOOD;
void f( A far * ) GOOD;
void f( B near * ) GOOD;
void f( B far * ) GOOD;
void f( C near * ) GOOD;
void f( C far * ) GOOD;

void g( A near *npa,
	A far *fpa,
	B near *npb,
	B far *fpb,
	C near *npc,
	C far *fpc )
{
    f( npa );
    f( fpa );
    f( npb );
    f( fpb );
    f( npc );
    f( fpc );
}
int main( void ) {
    g( 0, 0, 0, 0, 0, 0 );
    CHECK_GOOD( 63 );
    return errors != 0;
}
