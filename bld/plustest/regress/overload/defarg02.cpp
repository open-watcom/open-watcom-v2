#include "dump.h"


void f( char c ) BAD;
void f( int i, char c = 'a' ) GOOD;

void g( short s )
{
    f( s );	// should chose f( int i );
}
int main( void ) {
    g( 1 );
    CHECK_GOOD( 5 );
    return errors != 0;
}
