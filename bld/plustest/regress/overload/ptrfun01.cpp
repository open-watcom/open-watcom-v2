#include "dump.h"


typedef int (far *fpFTicRi)( int, char );
void f( fpFTicRi, double ) GOOD;
void f( ... ) BAD;

int far h( int, char ) { return 0; };

void g( double a )
{
    f( h, a );
}
int main( void ) {
    g( 1.0 );
    CHECK_GOOD( 5 );
    return errors != 0;
}
