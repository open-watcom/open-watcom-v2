#include "dump.h"


// from T(args)	to T(*)(args)
void fpfi( ... ) BAD;
typedef int (far *pFTiRi)( char, int );
void fpfi( pFTiRi ) GOOD;
int  (far hpfi)( char, int ) { return 0; };

void gpfi( void )
{
    fpfi( hpfi );
}

int main( void ) {
    gpfi();
    CHECK_GOOD( 7 );
    return errors != 0;
}
