#include "fail.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/* Initialization involving flexible array members */

typedef struct {
    int     i;
} A;

typedef struct {
    long    l;
    A       a[];
} B;

static B b1 = { 1, { {2}, {3}, {4}, {5}, {6}, {7} } };

static B b2 = { 1, { {2}, {3} } };

/* This isn't ISO C but OW accepts it as an extension (see Bug 626) */
typedef struct {
    B       *b[];
} C;
C c1 = { 0 };

int main( void ) {
    if( sizeof( B ) != sizeof( long ) ) fail( __LINE__ );
    if( sizeof( b1 ) != sizeof( long ) + 6 * sizeof( int ) ) fail( __LINE__ );
    if( sizeof( b2 ) != sizeof( long ) + 2 * sizeof( int ) ) fail( __LINE__ );
    if( offsetof( B, a ) != sizeof( long ) ) fail( __LINE__ );
    _PASS;
}
