/* MIX5C.C - This code shows how to access a FORTRAN
 *           common block from C.
 *
 * Compile: wcc /ml mix5c
 *          wcc386  mix5c
 */
#include <stdio.h>

#pragma aux put "^";
#pragma aux cblk "^";

#ifdef __386__
#define FAR
#else
#define FAR far
#endif

extern struct cb {
    long int i,j;
} FAR cblk;

void put( void )
{
    printf( "i = %ld\n", cblk.i );
    printf( "j = %ld\n", cblk.j );
    cblk.i++;
    cblk.j++;
}
