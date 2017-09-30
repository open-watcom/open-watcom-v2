#include "dump.h"

#pragma warning 5 10

int __far a;

void f( int __near & ) GOOD;
void f( ... ) BAD;

void g( void )
{
    f( a );
}
int main( void ) {
    g();
    CHECK_GOOD( 7 );
    return errors != 0;
}
