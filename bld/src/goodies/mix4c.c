/* MIX4C.C - This C function prints a string passed from
 *           FORTRAN.
 *
 * Compile: wcc /ml mix4c
 *          wcc386  mix4c
 */

#include <stdio.h>

void cstr( char *instring )
{
    printf( "%s\n", instring );
}
