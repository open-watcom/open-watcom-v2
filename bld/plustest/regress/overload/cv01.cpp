#include "dump.h"


void f( int ) GOOD;
void f( float ) GOOD;

void g( void )
{
    const int a = 3;
    volatile float b = 2.5;
    f( a );
    f( b );
}
int main( void ) {
    g();
    CHECK_GOOD( 9 );
    return errors != 0;
}
