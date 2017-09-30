#include "fail.h"
#include <limits.h>

#if INT_MAX > 65536

#define BUFFER_SIZE     (1024*73)

void foo( int *before, char *buffer, int *after ) {

    memset( buffer, 0xff, BUFFER_SIZE );
    if( *before != 0 ) fail( __LINE__ );
    if( *after != 1 ) fail( __LINE__ );
    *before = 1;
    *after = 0;
}

int main() {
    int         before;
    char        buffer[ BUFFER_SIZE ];
    int         after, i;

    before = 0;
    after = 1;
    for( i = BUFFER_SIZE - 1; i > 0; i -= 4096 ) {
        buffer[i] = 0;
    }
    foo( &before, &buffer[ 0 ], &after );
    if( before != 1 ) fail( __LINE__ );
    if( after != 0 ) fail( __LINE__ );
    _PASS;
}

#else
ALWAYS_PASS
#endif
