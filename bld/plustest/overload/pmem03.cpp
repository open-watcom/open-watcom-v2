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
    f( (int)0 );
    f( (unsigned)0 );
}
int main( void ) {
    g();
    CHECK_GOOD( 22 );
    return errors != 0;
}
