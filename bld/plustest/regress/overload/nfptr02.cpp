#include "dump.h"

#pragma warning 5 10

int far a;

void f( int near * ) GOOD;
void f( ... ) BAD;

void g( int far *a )
{
    f( a );
}
int main( void ) {
    g( &a );
    CHECK_GOOD( 7 );
    return errors != 0;
}
