#include "dump.h"

void f( int far & ) GOOD;
void f( ... ) BAD;

void g( int a )
{
    f( a );
}
int main( void ) {
    g( 1 );
    CHECK_GOOD( 3 );
    return errors != 0;
}
