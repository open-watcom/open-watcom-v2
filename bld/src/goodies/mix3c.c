/* MIX3C.C - This C function passes a string back to its
 *           calling FORTRAN program.
 *
 * Compile: wcc /ml mix3c
 *          wcc386  mix3c
 */

#include <string.h>

#pragma aux sendstr "^";

typedef struct descriptor {
    char          *addr;
    unsigned       len;
} descriptor;

void sendstr( descriptor *ftn_str_desc )
{
    ftn_str_desc->addr = "This is a C string";
    ftn_str_desc->len  = strlen( ftn_str_desc->addr );
}
