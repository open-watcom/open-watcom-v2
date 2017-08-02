#include "fail.h"
#include <limits.h>

const unsigned int maxlen = UINT_MAX;

int main( void )
{
    unsigned i;
    char *a = "hello";

    for ( i = 0; a[i] && i < maxlen; i++ );
    /* loop optimizer bug: the i < maxlen test
       overflowed in OW 1.0 and earlier */

    if ( i != 5 )
        _fail;
    _PASS;
}
