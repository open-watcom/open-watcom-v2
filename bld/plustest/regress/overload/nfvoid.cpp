#include "dump.h"

void f( void near *, void far * ) GOOD;
void f( ... ) BAD;

void g( int *a, int *b )
{
    f( a, b );
}
int main( void ) {
    int a;
    g( &a, &a );
    CHECK_GOOD( 3 );
    return errors != 0;
}
