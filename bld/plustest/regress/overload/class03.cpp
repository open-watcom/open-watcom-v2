#include "dump.h"


class A { public: int a; };
class B : public A { public: int b; };
class C : public B { public: int c; };

void f( A __near * ) GOOD;
void f( A __far * ) GOOD;
void f( B __near * ) GOOD;
void f( B __far * ) GOOD;
void f( C __near * ) GOOD;
void f( C __far * ) GOOD;

void g( A __near *npa,
        A __far *fpa,
        B __near *npb,
        B __far *fpb,
        C __near *npc,
        C __far *fpc )
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
