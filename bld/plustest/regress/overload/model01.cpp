#include "dump.h"


void f( double, char far *, int ) BAD;
void f( double, char near *, int ) GOOD;

void g( double a, char *b, int c )
{
    f( a, b, c );
}
int main( void ) {
    g( 1.0, 0, 2 );
    CHECK_GOOD( 5 );
    return errors != 0;
}
