/*
 * Make sure -ri is compatible with -oe (Bug 636)
 */

#include "fail.h"

char test_function( void )
{
    return( 'A' );
}

int main( void )
{
    test_function();
    _PASS;
}
