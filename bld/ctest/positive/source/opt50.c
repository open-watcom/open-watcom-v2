/*
 * OW 1.6 RISCifier broke the followingsimple code; fixed post-1.9
 */

#include "fail.h"

unsigned char   arr[80];
unsigned char   state;
unsigned int    res;

/* Try to get more or less controlled garbage into registers */
long dirty( long a, long b )
{
    return( a + b );
}

void test( void )
{
    static unsigned char idx = 7;

    if (state == 9) {
        res |= 8;
        arr[idx] = 1;
    } else {
        arr[idx] = 0;
    }
}

int main( void )
{
    int rc;

    arr[7] = 1;
    rc = dirty( 0x12345678, 0x87654321 );
    test();
    if( arr[7] ) _fail;
    _PASS;
}
