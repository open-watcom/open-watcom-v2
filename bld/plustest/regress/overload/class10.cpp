#include "dump.h"


class A {
    public:
	int ai;
	operator float () { GOOD; return 1.0; }
	operator int () { BAD; return 1; }
};

void f( char ) BAD;
void f( double ) GOOD;

void g( void )
{
    A a;
    f( a );	// f( (double) operator float( a ) );
}
int main( void ) {
    g();
    CHECK_GOOD( 19 );
    return errors != 0;
}
