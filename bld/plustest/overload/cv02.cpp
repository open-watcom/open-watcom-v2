#include "dump.h"

// u-d conversion that removes const...

class A {
    public:
	int ai;
	A( int const & ) GOOD;
};

void f( A ) GOOD;
void f( ... ) BAD;

void g( int const a )
{
    f( a );	// should pick f( A( a ) );
}
int main( void ) {
    g( 2 );
    CHECK_GOOD( 19 );
    return errors != 0;
}
