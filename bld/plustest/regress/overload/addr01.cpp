#include "dump.h"

void f( int ) GOOD;
void f( char ) GOOD;

void g( void )
{
    auto void (*a1)( int );
    auto void (*a2)( int );
    auto void (*b1)( char ) = &f;
    auto void (*b2)( char ) = f;
    a1 = &f;
    a2 = f; // has to have & ?
    a1( 3 );
    a2( 3 );
    b1( 4 );
    b2( 4 );
}
int main( void ) {
    g();
    CHECK_GOOD( 14 );
    return errors != 0;
}
