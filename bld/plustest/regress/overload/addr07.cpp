#include "dump.h"

void g( char ) BAD;
void g( short ) BAD;

void f( short ) GOOD;
void f( auto void (*)( char ) ) GOOD;

void foo2( void )
{
    auto void (*a)( short );
    a = f; 		// has to have & ?
    a( 1 );
    f( g );		// has to have & ?
}
int main( void ) {
    foo2();
    CHECK_GOOD( 13 );
    return errors != 0;
}
