#include "dump.h"


void f( char *, char *, char * ) GOOD;
void f( ... ) BAD;

void g( void )
{
    enum { a=0 };

    f( (int)0, (unsigned)0, a );
}
int main( void ) {
    g();
    CHECK_GOOD( 4 );
    return errors != 0;
}
