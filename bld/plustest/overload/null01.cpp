#include "dump.h"


void f( char * ) GOOD;
void f( ... ) BAD;

void g( void )
{
    f( (int)0 );
    f( (unsigned)0 );
}
int main( void ) {
    g();
    CHECK_GOOD( 8 );
    return errors != 0;
}
