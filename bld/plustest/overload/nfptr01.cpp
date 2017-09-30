#include "dump.h"

void f( int __far * ) GOOD;
void f( ... ) BAD;

void g( int __near *a )
{
    f( a );
}
int main( void ) {
    int a;
    g( &a );
    CHECK_GOOD( 3 );
    return errors != 0;
}
