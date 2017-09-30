#include "dump.h"

void f( int ) GOOD;
void f( char ) GOOD;

void (* g1( void ) )( int )
{
    GOOD;
    return &f;
}

void (* g2( void ) )( char )
{
    GOOD;
    return f;
}
int main( void ) {
    (*(g1()))( 0 );
    (*(g2()))( 0 );
    CHECK_GOOD( 29 );
    return errors != 0;
}
