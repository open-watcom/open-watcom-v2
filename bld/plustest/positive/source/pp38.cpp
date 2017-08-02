/* Verify that compiler doesn't falsely recognize include guards.
 * See bug 966. Been broken at least since version 10.5!
 */
#define _ENABLE_AUTODEPEND  /* Define the "include guard" */
#pragma pack( 8 )           /* Make sure to set reasonably high packing */
#include "t_pck2.h"         /* Set smaller packing */
#include "t_pck1.h"
#include "t_poppk.h"
#include "t_poppk.h"        /* If compiler is broken, this won't work */

typedef struct {
    short   s1;
    long    l1;
} S1;

int foo = sizeof( S1 );

#include "fail.h"

int main( void )
{
    if( sizeof( S1 ) != ( 2 * sizeof( long ) ) ) fail( __LINE__ );
    if( foo != sizeof( S1 ) ) fail( __LINE__ );
    _PASS;
}
