#include "dump.h"


void f( char *, char *, char * ) GOOD;
void f( ... ) BAD;

void g( void )
{
  f( (int)0, (unsigned)0, (long)0 );
}
int main( void ) {
    g();
    CHECK_GOOD( 4 );
    return errors != 0;
}
