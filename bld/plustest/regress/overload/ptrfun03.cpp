#include "dump.h"


void fpfi( ... ) BAD;
void fpfi( void * ) GOOD;
int  hpfi( char, int ) { return 0; };

void gpfi( void )
{
    fpfi( hpfi );
}

int main( void ) {
    gpfi();
    CHECK_GOOD( 5 );
    return errors != 0;
}
