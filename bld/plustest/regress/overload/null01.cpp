#include "dump.h"


void f( char * ) GOOD;
void f( ... ) BAD;

void g( void )
{
    enum { a=0 };

    f( (int)0 );
    f( (unsigned)0 );
    f( a );
}
int main( void ) {
    g();
    CHECK_GOOD( 12 );
    return errors != 0;
}
