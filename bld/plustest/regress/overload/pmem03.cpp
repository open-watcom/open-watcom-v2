#include "dump.h"


class B {
    public:
	int bi;
	char bc;
};

void f( ... ) BAD;
void f( char B:: *pbc ) GOOD;

void g( void )
{
    enum { a=0 };

    f( (int)0 );
    f( (unsigned)0 );
    f( a );
}
int main( void ) {
    g();
    CHECK_GOOD( 33 );
    return errors != 0;
}
