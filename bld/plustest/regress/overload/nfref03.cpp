#include "dump.h"

void f( int far & ) GOOD;
void f( ... ) BAD;

void g( int near & a )
{
    f( a );
}
int main( void ) {
    int a;
    g( a );
    CHECK_GOOD( 3 );
    return errors != 0;
}
